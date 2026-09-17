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
