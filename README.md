# meiga
Simulations Framework for Astroparticle Physics and Applications

# Introduction

It has been known for several decades that the Earth's surface constantly receives a flux of radiation resulting from the interaction of cosmic rays in the atmosphere. This radiation (in the form of particles) can be used for different purposes: scientific, such as the study of its origin and energy; or technological, such as muon tomography (or muonography), neutron studies of the ground, etc. To carry out these purposes, particle detectors and simulations are generally needed to evaluate the scope and reliability of a given project.

Meiga is a framework designed to develop astroparticle simulations and their possible applications. It is a set of classes written in C++ that connects the particle flux simulation with the response of a given detector through Geant4. The framework is designed with a hierarchical structure for data transfer and access and also provides an interface for the user to configure the parameters of the simulation or detector design.

# Installation

### Pre-requisites
Prior to install and use Meiga, the following packages need to be installed:

- **Geant4**:\
  Meiga uses Geant4 as a toolkit for simulating the interaction of particles with the detector. Current version used is **geant4-07.p03** and can be downloaded from the [source page](https://geant4.web.cern.ch/support/download_archive). Geant4 must be built with X11 OpenGL drivers (in order to enable visualization) and with DATA packages enabled: \
  `$cmake -DCMAKE_INSTALL_PREFIX=<path-to-install> -DGEANT4_INSTALL_DATA=ON -DGEANT_USE_OPENGL_X11=ON <path-to-source>`\
  See [Geant4 installation guide](https://geant4-userdoc.web.cern.ch/UsersGuides/InstallationGuide/html/) for details.

- **[boost](https://www.boost.org/)** (version > 1.75)\
  Can be installed via\
  `sudo apt-get install libboost-all-dev`

- **[nlohmann-json](https://github.com/nlohmann/json.git)** \
  For JSON parsers. Can be installed via:\
  `sudo apt-get install nlohmann-json-dev`
- **view3dscene** (or similar) for visualization of .wrl files.\
  `sudo apt-get install view3dscene`

### Setting your environment
Before installing Meiga, be sure that Geant4 and the DATA packages are correctly located in the system. Just add the following lines to your `~/.bashrc` file:\

`source $HOME/lib/geant4/install/bin/geant4.sh`\
`G4COMP="$HOME/lib/geant4/install/share/Geant4-10.7.2"`\
`export G4ABLA=$G4COMP/data/G4ABLA3.1`\
`export G4EMLOW=$G4COMP/data/G4EMLOW7.13`\
`export G4ENSDFSTATE=$G4COMP/data/G4ENSDFSTATE2.3`\
`export G4INCL=$G4COMP/data/G4INCL1.0`\
`export G4NDL=$G4COMP/data/G4NDL4.6`\
`export G4PARTICLEXS=$G4COMP/data/G4PARTICLEXS3.1.1`\
`export G4PII=$G4COMP/data/G4PII1.3`\
`export G4SAIDDATA=$G4COMP/data/G4SAIDDATA2.0`\
`export PhotonEvaporation=$G4COMP/data/PhotonEvaporation5.7`\
`export RadioactiveDecay=$G4COMP/data/RadioactiveDecay5.6`\
`export RealSurface=$G4COMP/data/RealSurface2.2`

### Build

1. Get the source code from the repository:\
`git clone https://gitlab.ahuekna.org.ar/muar/meiga.git` 
2. In the cloned directory, create the `build` and `install` directories
3. In the `build` directory type:\
`cmake -DCMAKE_INSTALL_PREFIX=../install ../src`
4. Then build and install:\
`make -jN; make install;`

# Framework Structure

# Input flux

# Example Applications
