
# musrSim plane-truth extension

This is a separate repository based on `TwinklyStar/musrSim-jp` commit `168ac52`
(which inherited SMS features). It adds primary-muon crossings of four fixed
world-z planes to ROOT `t1` and restores the EcoMug commands required by the
sim4 cosmic-muon macro. The original JP code is retained below for reference.
The PSI, SMS, and JP source repositories were reviewed; this extension stays
on the JP/Geant4 10.7.2 branch used by the current STL macros.

## Build on the cluster

```bash
source /home/liyifei/load_lcg.sh
cd /lustre/collider/liyifei/musrSim-plane-truth  # adjust to your upload location
cmake -S . -B build -DWITH_GEANT4_UIVIS=OFF
cmake --build build -j4
ctest --test-dir build --output-on-failure
export MUSRSIM_BIN="$PWD/build/musrSim_plane_truth"
```

The build still uses Geant4, ROOT, and CMake in the same style as SMS/JP. The
bundled EcoMug v2.1 header requires no separate cluster installation; see
`third_party/EcoMug/PROVENANCE.md` and its GPL-3.0 license. Keep the old
`musrSim_upgrade` executable and existing ROOT files for historical results.

## New macro and ROOT interface

```text
/musr/command truthPlaneZ 749 448 -272 -574
/gun/ecomug/useEcoMug true
/gun/ecomug/shapeConstruct sphere 1000 0 0 0 0
# optional: /gun/ecomug/constraints 10 10000 20 60 0 180
```

`truthPlaneZ` gives L1-L4 reference z in world mm, in strictly descending
order. When omitted, JP output is unchanged. When enabled, `t1` adds
`truthPlaneX/Y/Z[4]`, `truthPlaneCount[4]`, `truthPlaneRefZ[4]`, and
`truthPlanePrimaryMuonCount`. Only a primary (`ParentID=0`) muon (`PDG=±13`)
crossing downward is recorded. The first step crossing is retained for
diagnostics; count 0 or greater than 1, and a primary-muon count other than 1,
are invalid independent references. Missing coordinates are NaN. All
coordinates use world mm and arise from step-endpoint interpolation, not
detector hits or the initial momentum line. No detector volume or transport
step limit is added. With curved field propagation, endpoint interpolation
should be treated as a chord approximation.

In plane-truth mode, `t1` also supplies `parID` (generated primary PDG) and
`parIniPosX/Y/Z`, `parIniMomX/Y/Z` (generated world position in mm and momentum
in MeV/c). These match the initial-truth column names expected by the existing
analysis exporter. They are not substituted for actual downstream crossings.

EcoMug v2.1 interprets the five geometry numbers as plane width/height or
sphere radius/unused 0 or cylinder radius/height, then centre x/y/z, all in
mm. `constraints` takes momentum limits in MeV/c and angle limits in degrees.
The adapter converts momentum to/from EcoMug GeV/c and generates world-downward
directions using EcoMug's returned world polar angle (already measured from
positive z and greater than 90 degrees for downward muons). The selected
EcoMug seed is printed and stored as `ecoMugSeed` in
`t1`. EcoMug v2.1 uses a separate charge random engine, so this seed does not
guarantee bitwise reproduction of the complete charge sequence.

The sim4/sim5 job scripts in `muography_shine` require an absolute
`MUSRSIM_BIN` and write into `data_plane_truth/`; they do not overwrite the
old `data/`. Before submitting Condor jobs, create the new log directories and
export `MUSRSIM_BIN` in the submission environment (the submit files pass it
through with `getenv = True`):

```bash
cd /lustre/collider/liyifei/muography_shine/sim4_empty_truth
mkdir -p logs_plane_truth/{logs,out,err}
./run.sh 90 4090 1000                 # pilot; use a new run number/seed
root -l data_plane_truth/musr_90_4090.root
# ROOT prompt: t1->Print(); t1->GetEntries();
```

Run a matching sim5 pilot, then export all three event-aligned tables with
`analysis/extract_truth_hits.cpp` using its final `require_plane_truth=true`
argument. It also writes per-file `*_generation.txt`. Verify a small pilot
and generator distributions against the historical sample before any full
sim4/sim5 rerun. `storeOnlyEventsWithHits true` means `t1` covers stored hit
events; `geantParametersD[5]` supplies each run's generated-event count.
Move pilot ROOT files into a `data_plane_truth/pilot/` subdirectory before
exporting the production runs so they do not enter the full-run sample.

## Historical JP README

# musrsim-jp
Geant4 package for musrSim (Zhi Yuan J-PARC project dedicated)
It is transferred from musrsim-sms (Shanghai Muon Source dedicated)


# Tutorial

### Setup environment
This package based on Geant4 version `10.7.2`.

On `INPAC-cluster`, you can setup Geant4 enviroment with:

```
source  /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc11-opt/setup.sh
```
### Download and compile the package

```
git clone https://github.com/TwinklyStar/musrSim-jp.git
cd musrSim-jp
mkdir build
cd build
cmake ../
make -j4
```
### Create working directory

```
mkdir run
cd run
cp ../../run/1000_Laser.mac ../../run/visVRML.mac .
```
### Start simulation

```
../musrSim_jp 1000_Laser.mac test_run
```
### Run on condor
First copy condor scripts
```
cp ../../run/submit.condor ../../run/run.sh .
```
Then submit 
```
condor_submit submit.condor
```


# Updates
### 2021-11-30 (CC)
Add random seed offset, only take effect with `/musr/run/randomOption 1` in mac file.
With `randomOption=1`, Geant4 will take the current system time as the random seed. If there are mult-jobs submitted simultaneously, different jobs could have a same system time since have a same random seed. To avoid this, the random seed should be offset by a number.

Example:
```
# Set the offset as input parameter
../musrSim_jp 1003.mac name 10

# Set the offest in macro file
/musr/command SetRndSeedOffset 10
```
In this case, this random seed of this job will be offset by 10.
In practice, we submit me jobs with `condor`, each job will be offset by their job process number.


### 2021-11-25 (ML)
Enables the customization of output file name in `.mac` steering file

Example:
```
# set output file name
/musr/command SetOutputFileName myRootFile
```
And the output root file will be: `musrSim_myRootFile.root`

Moreover, you can specify the name as DEFAULT:
```
/musr/command SetOutputFileName DEFAULT
```
This is equivalent to running the macro without this command line


### 2021-11-17 (CC)
Enabled the customizing of crosssection factors on mac steering file, the default value is set to 1.0 if not specified

Example:
```
# set gmumu xsection factor to 1000.0
/musr/command G4EmExtraPhysics SetCrossSecFactor gmumuFactor 1000.0
```

Now can specify a name when launch the job:
```
../musrSim_jp 1003.mac name
```
The output file will be `musr_1003_name.root`.


### 2022-1-24 (ML)
Add branch:
```
det_edep_mun        # mu- energy deposition
det_kine_mup        # mu+ kinetic energy
det_kine_mun        # mu- kinetic energy
det_x(/y/z)_mup     # position where mu+ hits the detector
det_x(/y/z)_mun     # position where mu- hits the detector
```
You can turn off these branches by add following command in `.mac` file:
```
/musr/command rootOutput <branch_name> off
```


### 2022-4-2 (ML)
Add customization of 3 specific physic processes. Use command below in `.mac` file to control:
```
/musr/command G4EmExtraPhysics SetProcessState GammaNuclear on(off)
/musr/command G4EmExtraPhysics SetProcessState ElectroNuclear on(off)
/musr/command G4EmExtraPhysics SetProcessState GammaToMuPair on(off)
```

Fix bugs about abnormal larger kinetic energy


### 2022-4-30 (ML)
Add branch recording the parent track ID for particles recorded in detector hit
arrays: `det_VrtxPrtTrackID`. This is a Geant4 track ID, not a particle PDG ID.
It is aligned with the `det_*` arrays and is filled from the parent track ID of
the detector hit.

You can turn it off by adding following command in `.mac` file:
```
/musr/command rootOutput det_VrtxPrtTrackID off
```

### 2022-6-26 (ML)
Add customization of ElectroNuclear cross section factor in `.mac` file.\
Default value is 1.0\
For example:
```
# set electronuclear cross section factor to 1000
/musr/command G4EmExtraPhysics SetCrossSecFactor enFactor 1000.0
```

### 2022-7-4 (ML)
Enable importing geometry from `.stl` file in `.mac`.\
Example:
```
#parameter guide            name  input file  material  x y z   mother      rotation      det#
/musr/command construct stl Struc bunny.stl   G4_W      0 0 0   log_World   norot    dead 102
```

### 2022-7-11 (ML)
Enable setting random seed from `.mac` file, if random option is set as `5`.\
Example:
```
# Random seed option
/musr/run/randomOption 5

# Set random seed as 9999999
/musr/command SetRndSeed 9999999
```

### 2022-8-6 (ML)
Add cosmic muon energy distribution. You can turn it on/off in `.mac` file:
Example：
```
# Turn on cosmic muon distribution
/gun/cosmic true

# Turn off cosmic muon distribution
/gun/cosmic false
```
Once being turned on, it will overwrite all other settings for momentum and kinetic energy in `.mac` file.

### 2022-11-24 (ML & XT)
Add event display, which can store 1 specific event visualization each run.
To turn on this function, you should make following changes in `.mac` file in specific positions

Example: the macro showed below stores the visualization of 11th event (event number is 10). Lines with arrow are newly added
```
...
##### VISUALIZATION #####
/vis/drawOnlyToBeKeptEvents         # <----
/musr/command VisualizeEvent 10     # <----
/vis/disable                        # Do NOT comment this line
#/control/execute visFromToni.mac
#/control/execute visDawn101.mac
/control/execute visVRML.mac
...

##### BEAM ON #####
/run/beamon 100
/vis/enable                         # <----
/vis/reviewKeptEvents               # <----
...

```

### 2022-11-26 (ML)

Add root event selector. Only events that satisfy the selection will be stored\
You can customize your selection in `rootEventSelector::Selector()`, which is in `rootEventSelector.cc`\
A simple example is already provided in `rootEventSelector::Selector()`\
After adding your cut, be sure to **build again before running**.

You can turn your selector on by adding following command in `.mac` file:
```
/musr/command rootEventSelector on
```

### 2023-1-18 (ML)

Add momentum information of the first electron decayed from mu-.\
Note that these branches are only valid when initial particle is mu-.\
Branch name: `elIniMomX(/Y/Z)`

You can turn it off by adding following command in `.mac` file:
```
/musr/command rootOutput <branch_name> off
```

### 2023-5-28 (ML)

Add mu- captured by nucleus at rest physics process.\
You can turn on/off this process by adding following line in `.mac` file:

```
# This process is set to occur only when mu- is at rest
/musr/command process addProcess mu- G4MuonMinusCapture
```

### 2024-2-10 (ML)

Add stopping position information for the first positron decayed from mu+. The
code stores the track ID of the positron produced by `DecayWithSpin`, then saves
that same positron's final position when the track stops or is killed.\
Branch name: `posEndPosX(/Y/Z)`; unit is mm.

You can turn it off by adding following command in `.mac` file:
```
/musr/command rootOutput <branch_name> off
```

### 2026-5-14 (ML)

Update `FindROOT.cmake` ROOT version parsing so both old slash-style versions
such as `5.00/00` and dot-style versions such as `6.30.06` can be handled.

Add parent track ID output for particles recorded by special save volumes. When
a particle first enters a configured save volume, the ROOT tree now stores its
Geant4 parent track ID in `save_PrtTrackID`, aligned with the existing `save_*`
arrays. This complements `det_VrtxPrtTrackID`: `det_VrtxPrtTrackID` is for
normal detector-hit rows, while `save_PrtTrackID` is for special save-volume
rows.

The `det_VrtxPrtTrackID` branch has the usual `rootOutput` on/off control:
```
/musr/command rootOutput det_VrtxPrtTrackID off
```

There is no separate `rootOutput` on/off switch for `save_PrtTrackID` at this
time. The `save_*` branches, including `save_PrtTrackID`, are created together
when at least one special save volume is configured.
