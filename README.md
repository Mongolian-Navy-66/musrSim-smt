# musrSim-mst

**Muon scattering tomography simulation with independent MC plane crossings.**

[简体中文](README.zh-CN.md) · [Notices and licenses](NOTICE.md) · [Contributing](CONTRIBUTING.md)

`musrSim-mst` is a source fork of [musrSim-jp](https://github.com/TwinklyStar/musrSim-jp) at commit `168ac52`. That branch inherited the [SMS fork](https://github.com/kimsiang/musrsim-sms) and includes STL detector geometry and a cluster-oriented CMake build. The [PSI musrSim project](https://www.psi.ch/en/lmu/geant4-simulations) is the original upstream. This fork adds a passive record of where primary muons cross four fixed world-`z` planes. It also vendors [EcoMug v2.1](https://github.com/dr4kan/EcoMug/tree/v2.1) for the `/gun/ecomug/...` commands used by the companion cosmic-muon macro.

The executable is named **`musrSim_mst`**. The original JP README and historical build notes are preserved in [`docs/upstream/`](docs/upstream/README-musrSim-jp.md). This repository contains source code and small unit tests; it does not include production ROOT files, a measured detector-resolution claim, or a replacement for the historical `musrSim_upgrade` binary.

## Build

The JP baseline uses Geant4 10.7.2, ROOT, CMake, and C++14. Use an environment that exposes Geant4 and ROOT to CMake. For example, on a cluster with the LCG 101 view:

```bash
source /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc11-opt/setup.sh
cmake -S . -B build -DWITH_GEANT4_UIVIS=OFF
cmake --build build -j4
ctest --test-dir build --output-on-failure
./build/musrSim_mst path/to/your.mac
```

If your cluster provides its own environment script (such as `load_lcg.sh`), source that instead. The build follows the JP/SMS CMake layout. The two CTest targets exercise plane interpolation and the vendored EcoMug API without requiring a production simulation. A complete Geant4/ROOT build and sim4/sim5 pilot must still be checked in the target environment.

## Record independent plane truth

Add this line to a steering macro **before** `/run/beamOn`:

```text
/musr/command truthPlaneZ 749 448 -272 -574
```

The four numbers are L1–L4 world-`z` coordinates in **mm**, strictly descending. They are example values for the companion sim4/sim5 geometry; use the actual fixed reference planes for another detector. Omitting this command retains the JP branch layout and `musrSim_jp` ROOT filename prefix; configuring it uses the `musrSim_mst` prefix. When configured, ROOT tree `t1` adds:

| Branch | Meaning |
| --- | --- |
| `truthPlaneX/Y/Z[4]` | First downward crossing coordinates in world mm; NaN if absent. |
| `truthPlaneCount[4]` | Downward crossing count at each plane. |
| `truthPlaneRefZ[4]` | Configured plane positions in world mm. |
| `truthPlanePrimaryMuonCount` | Distinct primary muon tracks observed in the event. |
| `parID`, `parIniPosX/Y/Z`, `parIniMomX/Y/Z` | Generated primary PDG, position in mm, and momentum in MeV/c for the companion exporter. |

The stepping action considers only `ParentID == 0`, PDG `±13`, and `z_pre > z_plane >= z_post`. It interpolates the crossing from the step endpoints. A layer is usable as independent truth only when its crossing count is **exactly one** and the event has **exactly one** primary muon. Repeated crossings retain the first coordinate for diagnosis; zero crossings retain NaN. The interpolation is a chord approximation if the trajectory curves significantly inside a step. These coordinates are independent of detector hits and of a straight line extrapolated from the initial momentum.

The existing `storeOnlyEventsWithHits true` setting still stores only hit events in `t1`. `geantParametersD[5]` records the generated event count for each ROOT file; do not use the `t1` row count as that denominator.

## EcoMug macro interface

```text
/gun/ecomug/useEcoMug true
/gun/ecomug/shapeConstruct sphere 1000 0 0 0 0
/gun/ecomug/seed 6000
# optional: /gun/ecomug/constraints 10 10000 20 60 0 180
```

`shapeConstruct` accepts `plane width height x y z`, `sphere radius 0 x y z`, or `cylinder radius height x y z`; lengths are mm. `constraints` accepts momentum minimum/maximum in MeV/c, polar-angle minimum/maximum in degrees, then azimuth minimum/maximum in degrees. The optional positive integer `seed` makes the complete EcoMug primary sequence reproducible; the local v2.1 patch seeds both the kinematic and charge engines. Without it, the historical CLHEP-derived initialization is retained. The adapter converts EcoMug's GeV/c momentum to Geant4 units and uses EcoMug's returned world direction directly, including its downward `z` component. The chosen seed is logged and stored as `ecoMugSeed` in `t1`. See [`third_party/EcoMug/PROVENANCE.md`](third_party/EcoMug/PROVENANCE.md).

## Optional diagnostic truth

Add `/musr/command storeDiagnosticTruth true` before `/run/beamOn` to create a `diagnosticTruth` tree aligned one-to-one with stored `t1` events. The default is `false`, in which case the tree is absent and no extra event data are collected. The diagnostic tree stores dynamic vectors for every track and every nonzero detector-Edep step: track/parent IDs, PDG, creator process, vertex volume and kinematics; and detector/track IDs, Edep, pre/post coordinates, length, time, kinetic energy, and the original Geant4 process name. `runID` and `eventID` provide the event key. The same hit-only and positive-weight predicate controls both trees; rejected events are absent from both.

Diagnostic recording only reads Geant4 state and does not intentionally consume random numbers. Validate this for a production build by running identical macros with the switch off/on and comparing every `t1` value. For exact comparison, use deterministic transport initialization such as `/musr/run/randomOption 2` and an explicit `/gun/ecomug/seed` because historical `randomOption 1` includes wall-clock time.

The lost-source `musrSim_upgrade` executable may have used a different EcoMug version or adapter. Compare generated-position, momentum, direction, and charge distributions statistically before interpreting a new sample alongside historical data. With hit-only storage, that comparison is conditional on the event being stored.

## Companion analysis and validation

The separate `muography_shine` analysis project contains sim4/sim5 macros, run scripts, `analysis/extract_truth_hits.cpp`, and `analysis/validate_layer_position.py`. Its scripts require `MUSRSIM_BIN` to be the absolute path of `build/musrSim_mst` and place new ROOT files in `data_plane_truth/`, leaving the old `data/` directory intact. The extractor writes aligned hits, initial truth, crossings, and per-file generation-count tables keyed by `Entry/SourceFile/SourceEntry/EventID`.

Run a small sim4 and sim5 pilot first. Check `t1->Print()`, four reference `z` values, missing and repeated crossings, hit branches, STL geometry, and row identities. Validate a bent trajectory against actual downstream steps. Then compare generator distributions, run the full sample, and report four methods only on their common successful `Entry` subset. `det_xyz_ideal` is an MC-coordinate benchmark; `edep_strip_proxy` is a strip-energy readout proxy. The two legacy fit methods remain historical baselines. The companion project's `analysis/README_layer_position_validation.md` gives the exact sequence.

## Credits and licenses

The repository retains upstream Git history and copyright notices. GPL-covered musrSim and EcoMug portions are distributed under GPLv3 because EcoMug v2.1 is GPLv3 and musrSim headers permit GPLv2 or later; other third-party files retain their own notices. See [`NOTICE.md`](NOTICE.md), the top-level [`LICENSE`](LICENSE), upstream [`COPYING`](COPYING), and the licenses in `third_party/` and `licenses/`. This product includes software developed by Members of the Geant4 Collaboration ( http://cern.ch/geant4 ).
