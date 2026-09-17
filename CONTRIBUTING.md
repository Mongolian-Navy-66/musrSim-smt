# Contributing / 参与开发

## English

1. Open an issue describing the geometry, macro commands, expected ROOT branches, and the musrSim-mst commit used. Do not upload large ROOT/data files or private cluster paths.
2. Keep `truthPlaneZ` passive: it must not alter geometry, tracking cuts, random-number consumption, or reconstruction fits. Preserve the unconfigured JP output layout and document any new ROOT branch, unit, and invalid state.
3. Build with Geant4/ROOT and run `ctest --test-dir build --output-on-failure`. Small tests can also compile directly with a C++14 compiler:

   ```bash
   c++ -std=c++14 -Iinclude tests/primary_muon_plane_truth_test.cc -o /tmp/primary_muon_plane_truth_test
   /tmp/primary_muon_plane_truth_test
   c++ -std=c++14 -Ithird_party/EcoMug tests/ecomug_api_test.cc -o /tmp/ecomug_api_test
   /tmp/ecomug_api_test
   ```

4. For changes that affect simulation output, include a small sim4/sim5 pilot showing branch types, four plane positions, crossing counts, `geantParametersD[5]`, and `Entry` alignment with the companion exporter. Report generator distributions when EcoMug behavior changes. Do not present ideal MC hit coordinates as detector resolution.
5. Update both [`README.md`](README.md) and [`README.zh-CN.md`](README.zh-CN.md) when the public interface, build steps, or interpretation changes. Retain third-party source and license notices.

GitHub Actions checks only the two dependency-free C++ tests. Full Geant4/ROOT and ROOT export validation must run in an environment with those dependencies.

## 中文

1. 提交 issue 时写明几何、宏命令、预期 ROOT 分支和使用的 musrSim-mst commit。不要上传大体积 ROOT/数据文件或私人集群路径。
2. 保持 `truthPlaneZ` 为被动记录功能：不得改变几何、轨迹切选、随机数消耗或重建拟合。未配置时须保持 JP 原有输出布局；新增 ROOT 分支须说明单位和无效状态。
3. 在有 Geant4/ROOT 的环境中编译并运行 `ctest --test-dir build --output-on-failure`。上面的两个 C++14 命令也可独立编译小型测试。
4. 改变模拟输出时，附上小样本 sim4/sim5 核对结果，包括分支类型、四层平面、穿越次数、`geantParametersD[5]`，以及与配套提取器的 `Entry` 对齐。EcoMug 行为变化还须比较生成分布。不要把理想 MC hit 坐标写成真实探测器分辨率。
5. 公共接口、编译步骤或物理解释变化时，同步更新 [`README.md`](README.md) 与 [`README.zh-CN.md`](README.zh-CN.md)，并保留第三方来源和许可声明。

GitHub Actions 只检查两个不依赖 Geant4/ROOT 的 C++ 小测试；完整模拟与 ROOT 导出仍须在有依赖的环境中验收。
