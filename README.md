# meiga
Simulations Framework for Astroparticle Physics and Applications

# Introduction

It has been known for several decades that the Earth's surface constantly receives a flux of radiation resulting from the interaction of cosmic rays in the atmosphere. This radiation (in the form of particles) can be used for different purposes: scientific, such as the study of its origin and energy; or technological, such as muon tomography (or muonography), neutron studies of the ground, etc. To carry out these purposes, particle detectors and simulations are generally needed to evaluate the scope and reliability of a given project.

Meiga is a framework designed to develop astroparticle simulations and their possible applications. It is a set of classes written in C++ that connects the particle flux simulation with the response of a given detector through Geant4. The framework is designed with a hierarchical structure for data transfer and access and also provides an interface for the user to configure the parameters of the simulation or detector design.

# Installation

### Pre-requisites
Prior to install and use Meiga, the following packages need to be installed:

- **Geant4**:\
  Meiga uses Geant4 as a toolkit for simulating the interaction of particles with the detector. Current version used is **geant4-07.p03** and can be downloaded from the [source page](https://geant4.web.cern.ch/support/download_archive). Geant4 must be installed with X11 OpenGL drivers in order to enable visualization.

- **boost** (version > 1.75)

- **nlohmann-json** \
  For JSON parsers. Can be installed via:\
  `sudo apt-get install nlohmann-json-dev`
- **view3dscene** (or similar) for visualization of .wrl files

# Framework Structure

# Input flux

# Example Applications
