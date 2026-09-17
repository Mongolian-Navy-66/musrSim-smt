# Source, copyright, and license notices / 来源、版权与许可声明

## English

- **musrSim lineage:** This repository retains the Git history and notices of the [JP fork](https://github.com/TwinklyStar/musrSim-jp) at `168ac52`, which inherited the [SMS fork](https://github.com/kimsiang/musrsim-sms), derived from [PSI musrSim](https://www.psi.ch/en/lmu/geant4-simulations). musrSim source headers generally permit GNU GPL version 2 **or any later version**. The original GPLv2 license text is retained in [`COPYING`](COPYING).
- **EcoMug:** [`third_party/EcoMug/EcoMug.h`](third_party/EcoMug/EcoMug.h) is upstream EcoMug v2.1 (`9721372`), licensed under GNU GPL version 3. Its license is in [`third_party/EcoMug/LICENSE`](third_party/EcoMug/LICENSE) and its exact source hash is in [`PROVENANCE.md`](third_party/EcoMug/PROVENANCE.md). The top-level [`LICENSE`](LICENSE) is the same GPLv3 text. The GPL-covered musrSim and EcoMug code in this combined program is distributed under GPLv3; the original notices remain in place. The [GNU GPL compatibility FAQ](https://www.gnu.org/licenses/gpl-faq.html#v2v3Compatibility) explains the GPLv2-or-later/GPLv3 combination.
- **CADMesh:** [`include/CADMesh.hh`](include/CADMesh.hh) carries Christopher M. Poole's MIT notice and full MIT permission text in its header. Keep that notice with redistributed copies.
- **Geant4-derived files:** Files whose headers identify Geant4 Collaboration copyright retain the Geant4 Software License and disclaimer. A copy of the [official Geant4 license](https://github.com/Geant4/geant4/blob/master/LICENSE) is included in [`licenses/GEANT4-LICENSE`](licenses/GEANT4-LICENSE) (retrieved 2026-09-17; SHA-256 `E2E3B456D6F939A98438EB22C6B7AC6AD4306321010493AC6E014980DFCA567F`). **This product includes software developed by Members of the Geant4 Collaboration ( http://cern.ch/geant4 ).**
- **External dependencies:** Geant4 and ROOT are required to build the simulator. Their installed distributions are not bundled here and retain their own licenses.

The archived JP README and build notes under [`docs/upstream/`](docs/upstream/) are historical material. Their executable names and cluster paths are not current instructions for this fork.

## 中文

- **musrSim 来源：**本仓库保留 [JP 修改版](https://github.com/TwinklyStar/musrSim-jp) `168ac52` 的 Git 历史和声明；JP 继承 [SMS 修改版](https://github.com/kimsiang/musrsim-sms)，源于 [PSI musrSim](https://www.psi.ch/en/lmu/geant4-simulations)。musrSim 源文件声明通常允许 GNU GPL 第 2 版**或任意更新版本**。上游 GPLv2 全文保存在 [`COPYING`](COPYING)。
- **EcoMug：**[`third_party/EcoMug/EcoMug.h`](third_party/EcoMug/EcoMug.h) 来自 v2.1（`9721372`），采用 GNU GPLv3。许可全文见 [`third_party/EcoMug/LICENSE`](third_party/EcoMug/LICENSE)，确切源码哈希见 [`PROVENANCE.md`](third_party/EcoMug/PROVENANCE.md)。顶层 [`LICENSE`](LICENSE) 是同一 GPLv3 文本。组合程序中受 GPL 约束的 musrSim 与 EcoMug 代码按 GPLv3 分发，同时保留原有版权声明。[GNU 兼容性说明](https://www.gnu.org/licenses/gpl-faq.html#v2v3Compatibility)解释了 GPLv2-or-later 与 GPLv3 的组合。
- **CADMesh：**[`include/CADMesh.hh`](include/CADMesh.hh) 在文件头保留 Christopher M. Poole 的 MIT 版权与完整许可文本；再分发时须保留。
- **Geant4 衍生文件：**在文件头标明 Geant4 Collaboration 版权的文件沿用 Geant4 Software License 与免责声明。仓库的 [`licenses/GEANT4-LICENSE`](licenses/GEANT4-LICENSE) 保存[官方许可](https://github.com/Geant4/geant4/blob/master/LICENSE)副本（获取于 2026-09-17；SHA-256 `E2E3B456D6F939A98438EB22C6B7AC6AD4306321010493AC6E014980DFCA567F`）。**本产品包含由 Geant4 Collaboration 成员开发的软件（http://cern.ch/geant4）。**
- **外部依赖：**构建模拟器需要 Geant4 与 ROOT；其安装包不随本仓库提供，并遵循各自许可。

[`docs/upstream/`](docs/upstream/) 中的 JP README 与旧构建资料仅供追溯；其中的程序名和集群路径不适用于本版本。
