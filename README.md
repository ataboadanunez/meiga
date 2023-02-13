# meiga
Simulations Framework for Astroparticle Physics and Applications

# Introduction
Meiga is a framework designed to develop astroparticle simulations and their possible applications. It is a set of classes written in C++ that connects the particle flux simulation with the response of a given detector through Geant4. The framework is designed with a hierarchical structure for data transfer and access and also provides an interface for the user to configure the parameters of the simulation or detector design.

# Getting started

### Pre-requisites
Prior to install and use Meiga, the following packages need to be installed:

- **Geant4**:\
  Meiga uses Geant4 as a toolkit for simulating the interaction of particles with the detector. Current version used is **geant4-07.p03** and can be downloaded from the [source page](https://geant4.web.cern.ch/support/download_archive). Geant4 must be built with X11 OpenGL drivers (in order to enable visualization) and with DATA packages enabled:
```bash
$cmake -DCMAKE_INSTALL_PREFIX=<path-to-install> -DGEANT4_INSTALL_DATA=ON -DGEANT_USE_OPENGL_X11=ON <path-to-source>
```
  See [Geant4 installation guide](https://geant4-userdoc.web.cern.ch/UsersGuides/InstallationGuide/html/) for details.

- **[boost](https://www.boost.org/)** (version > 1.75)\
  Can be installed via\
```bash
sudo apt-get install libboost-all-dev
```

- **[nlohmann-json](https://github.com/nlohmann/json.git)** \
  For JSON parsers. Can be installed via:
```bash
  sudo apt-get install nlohmann-json-dev
```
- **view3dscene** (or similar) for visualization of .wrl files:
```bash
  sudo apt-get install view3dscene
```

### Setting your environment
Before installing Meiga, be sure that Geant4 and the DATA packages are correctly located in the system. Just add the following lines to your `~/.bashrc` file:
```bash
# assume your Geant4 installation is under $HOME/lib/geant4
# source the Geant4 environment
source $HOME/lib/geant4/install/bin/geant4.sh
# export all DATA packages
G4COMP="$HOME/lib/geant4/install/share/Geant4-10.7.2"
export G4ABLA=$G4COMP/data/G4ABLA3.1
export G4EMLOW=$G4COMP/data/G4EMLOW7.13
export G4ENSDFSTATE=$G4COMP/data/G4ENSDFSTATE2.3
export G4INCL=$G4COMP/data/G4INCL1.0
export G4NDL=$G4COMP/data/G4NDL4.6
export G4PARTICLEXS=$G4COMP/data/G4PARTICLEXS3.1.1
export G4PII=$G4COMP/data/G4PII1.3
export G4SAIDDATA=$G4COMP/data/G4SAIDDATA2.0
export PhotonEvaporation=$G4COMP/data/PhotonEvaporation5.7
export RadioactiveDecay=$G4COMP/data/RadioactiveDecay5.6
export RealSurface=$G4COMP/data/RealSurface2.2
```
### Installation

1. Get the source code from the repository:
```bash
git clone https://gitlab.ahuekna.org.ar/muar/meiga.git
``` 
2. In the cloned directory, create the `build` and `install` directories
3. In the `build` directory type:
```bash
cmake -DCMAKE_INSTALL_PREFIX=../install ../src
```
4. Then build and install:
```bash
make -jN; make install;
```

# Framework Structure

The Meiga framework has a hierarchical structure that allows easy data access. 


- Event:\
The _Event_ is at the top of the structure and is used as a main vehicle for data flowing. In this sense, an _Event_ object is often passed as argument of the functions in the framework in such a way that data can be accessed to in any part of the code.
- Detector:\
The _Detector_ class provides an iterface to the _Event_ from which de detector description can be read. It contains the necessary methods for setting the detector properties and the functions to read these properties from configuration files, i.e., the conection between User and Geant4.
- SimData:\
Data from simulation is stored in the class _SimData_. This class has a hierarchical strcuture itself  for accessing data at different levels, for example, energy deposition of particles in a detector (_DetectorSimData_) or signal produced in a given optical sensor (_OptDeviceSimData_).

An example of how data can be accessed would be:

```cpp
double len = theEvent.GetDetector(detId).GetBarLength();
```

returns the length of a scintillator bar of a given detector with id `detId`.

```cpp
vector<double> Edep = theEvent.GetSimData().GetDetectorSimData(detId).GetEnergyDeposited();

```

returns the energy deposited of particles in that particular detector.

- G4Models:\
Everything related to the Geant4 _DetectorConstruction_ which can be described in a general way independently of any application is stored in the _G4Models_. For example, definition of materials, compounds and their physical properties but also detectors are described as independent classes within _G4Models_. 

The strcuture and workflow of the Meiga framework is shown in the diagram below. The _Meiga_ classes are represented in green while external packages (Geant4 and generators of input farticles) are represented in blue. The gray area shows the workflow of an _Application_. Arrows indicate the flow of information and whether a process is done automatically or can be configurable / done by the user.

![Workflow](src/Documentation/workflow.png)*Structure of the Meiga framework*

# Input flux

The framework takes as input a flux of particles which is stored in form of a _particle vector_. Each element of the vector is of the type `Particle` and contains all the information related to the particle which will be propagated (injected) and simulated. At the current version, the flux of particles can be injected from two sources: reading an existing file (using the ARTI framework), or randomly generated (using the EcoMug library).

### The ARTI input

Obtaining a more realistic particle flux may be relevant in order to obtain higher accuracy in our simulated data. For this purpose, the [ARTI framework](https://github.com/lagoproject/arti#top) can be used to calculate the flux of particles generated by cosmic-ray-induced showers in the atmosphere.   

The flux of galactic cosmic rays is given by
```math
\Phi(E_{p}, Z, A, \Omega) = j_{0}(Z, A) \left( \frac{E_{p}}{E_{0}}\right)^{\alpha(E_{p}, Z, A)}, 
``` 
where $`E_{p}`$ is the energy of the primary cosmic ray of atomic number _Z_ and mass number _A_, $`\Omega`$ is the solid angle, $`j_{0}`$ is a normalization parameter, and $`E_0 = 10^3~\mathrm{GeV}`$. The spectral index $`alpha`$ is consider independent of the primary energy for a wide range of energies.


### The EcoMug input

# Example Applications
