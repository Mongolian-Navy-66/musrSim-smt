# musrSim-mst

**用于 muon 散射层析成像、记录独立 MC 层面交点的 musrSim 源码仓库。**

[English](README.md) · [来源与许可证](NOTICE.md) · [参与开发](CONTRIBUTING.md)

本仓库以 [musrSim-jp](https://github.com/TwinklyStar/musrSim-jp) 的 `168ac52` 为基底，保留其 Git 历史。JP 分支继承了 [SMS 修改版](https://github.com/kimsiang/musrsim-sms)，含 STL 探测器几何与适合集群的 CMake 编译方式；[PSI musrSim](https://www.psi.ch/en/lmu/geant4-simulations) 是原始上游。本版本被动记录主 muon 穿越四个固定世界坐标 `z` 平面的实际位置，并集成 [EcoMug v2.1](https://github.com/dr4kan/EcoMug/tree/v2.1)，支持配套宇宙线宏中的 `/gun/ecomug/...` 命令。

新可执行文件名为 **`musrSim_mst`**。JP 原版 README 和旧构建资料保存在 [`docs/upstream/`](docs/upstream/README-musrSim-jp.md)。本仓库提供源码与小型单元测试；不包含生产 ROOT 数据，也不把理想 MC 坐标解释为实际探测器分辨率。旧 `musrSim_upgrade` 源码已丢失，本版本不能声称逐事件复现它。

## 编译

JP 基底使用 Geant4 10.7.2、ROOT、CMake 和 C++14。先加载能让 CMake 找到 Geant4、ROOT 的环境。例如，集群提供 LCG 101 时：

```bash
source /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc11-opt/setup.sh
cmake -S . -B build -DWITH_GEANT4_UIVIS=OFF
cmake --build build -j4
ctest --test-dir build --output-on-failure
./build/musrSim_mst path/to/your.mac
```

若集群已有 `load_lcg.sh`，可改为加载该脚本。编译流程沿用 JP/SMS 的 CMake 方式。两个 CTest 目标分别检查交点插值和 EcoMug API；仍需在目标集群完成 Geant4/ROOT 全程序编译以及 sim4、sim5 小样本验收。

## 记录独立的层面真值

在 steering 宏的 `/run/beamOn` **之前**加入：

```text
/musr/command truthPlaneZ 749 448 -272 -574
```

四个数依次为 L1–L4 的世界系 `z`，单位 **mm**，必须严格递减。这组值适用于配套 sim4/sim5 的参考平面；其他几何需填写自己的固定层面。不配置时，保持 JP 基底的分支布局和 `musrSim_jp` ROOT 文件名前缀；配置后改用 `musrSim_mst` 前缀，ROOT `t1` 增加：

| 分支 | 含义 |
| --- | --- |
| `truthPlaneX/Y/Z[4]` | 首次向下穿越的世界系坐标，单位 mm；缺失写 NaN。 |
| `truthPlaneCount[4]` | 各平面向下穿越次数。 |
| `truthPlaneRefZ[4]` | 配置的四个平面 `z`，单位 mm。 |
| `truthPlanePrimaryMuonCount` | 本事件观测到的不同主 muon 轨迹数。 |
| `parID`、`parIniPosX/Y/Z`、`parIniMomX/Y/Z` | 主粒子 PDG、初始位置 mm、初始动量 MeV/c，供配套提取器使用。 |

stepping action 只处理 `ParentID == 0`、PDG 为 `±13`、且满足 `z_pre > z_plane >= z_post` 的 step，并在两端点间插值。只有四层各恰好穿越一次、且主 muon 数恰好为 1 的事件，才能作为有效的独立交点真值。重复穿越保留首次坐标供诊断，零次穿越保留 NaN。轨迹若在一个 step 内明显弯曲，端点插值只是弦线近似。这些坐标不取自 hit，也不是初始动量直线的外推。

原有 `storeOnlyEventsWithHits true` 设置仍令 `t1` 只保存有 hit 的事件；每个 ROOT 的生成数记录在 `geantParametersD[5]`，不能用 `t1` 行数代替。

## EcoMug 宏接口

```text
/gun/ecomug/useEcoMug true
/gun/ecomug/shapeConstruct sphere 1000 0 0 0 0
/gun/ecomug/seed 6000
# 可选：/gun/ecomug/constraints 10 10000 20 60 0 180
```

`shapeConstruct` 支持 `plane width height x y z`、`sphere radius 0 x y z` 或 `cylinder radius height x y z`，长度单位为 mm。`constraints` 依次给出动量下/上限（MeV/c）、极角下/上限（度）、方位角下/上限（度）。可选正整数 `seed` 使完整 EcoMug 主粒子序列可重复；本地 v2.1 补丁同时播种运动学与电荷两个引擎。不设置时保留历史的 CLHEP 派生初始化。适配层把 EcoMug 的 GeV/c 动量转换为 Geant4 单位，直接采用 EcoMug 返回的世界系方向，其中 `z` 分量已表示向下运动。种子写入日志和 `t1` 的 `ecoMugSeed`。来源与固定版本见 [`third_party/EcoMug/PROVENANCE.md`](third_party/EcoMug/PROVENANCE.md)。

## 可选诊断真值

在 `/run/beamOn` 前加入 `/musr/command storeDiagnosticTruth true`，即可创建与已存储 `t1` 事件一一对应的 `diagnosticTruth` 树。默认严格为 `false`：此时不创建该树，也不收集额外事件数据。诊断树使用动态 vector 保存所有轨迹及所有非零探测器 Edep step：包括 track/parent ID、PDG、产生过程、顶点体积和运动学，以及探测器/track ID、Edep、pre/post 坐标、步长、时间、动能和 Geant4 原始过程名。`runID`、`eventID` 是事件键。两棵树使用相同的命中与正权重保存条件，被拒绝事件都不会写入。

诊断记录只读取 Geant4 状态，不应消耗随机数。生产构建须用相同宏分别关闭/开启该开关，并逐值比较全部 `t1` 分支。精确对照应使用确定性的输运初始化（例如 `/musr/run/randomOption 2`）和显式 `/gun/ecomug/seed`；历史 `randomOption 1` 含墙钟时间，不能用于逐事件复现。

失去源码的旧 `musrSim_upgrade` 可能使用不同 EcoMug 版本或适配方式。新旧样本并用之前，需要统计核对生成起点、动量、方向和电荷分布。若只保存有 hit 事件，比较对象也是“已存储事件”的条件分布。

## 配套分析与验收

独立的 `muography_shine` 分析项目提供 sim4/sim5 宏、运行脚本、`analysis/extract_truth_hits.cpp` 和 `analysis/validate_layer_position.py`。脚本要求 `MUSRSIM_BIN` 指向 `build/musrSim_mst` 的绝对路径；新 ROOT 放入 `data_plane_truth/`，保留旧 `data/`。提取器按 `Entry/SourceFile/SourceEntry/EventID` 对齐导出 hits、初始 truth、交点和每文件生成数。

先各跑少量 sim4、sim5：核对 `t1->Print()`、四层参考 `z`、缺失与重复计数、hit 分支、STL 几何及事件关联；用真实折线轨迹检查下游交点。再比较生成分布、运行全量样本，并只在四种方法共同成功的 `Entry` 子集上对照。`det_xyz_ideal` 是理想 MC 坐标基准；`edep_strip_proxy` 是条级 Edep 读出代理；两种遗留拟合方法仅作历史对照。具体命令见配套项目的 `analysis/README_layer_position_validation.md`。

## 致谢与许可

本仓库保留上游 Git 历史及版权声明。由于 EcoMug v2.1 为 GPLv3，而 musrSim 相关文件允许 GPLv2 或更新版本，受 GPL 约束的 musrSim 与 EcoMug 部分按 GPLv3 条款分发；其他第三方文件保留各自许可。详见 [`NOTICE.md`](NOTICE.md)、顶层 [`LICENSE`](LICENSE)、上游 [`COPYING`](COPYING)、`third_party/` 与 `licenses/`。本产品包含由 Geant4 Collaboration 成员开发的软件（http://cern.ch/geant4）。
