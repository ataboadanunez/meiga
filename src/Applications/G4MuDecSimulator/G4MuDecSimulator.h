#ifndef G4MuDecSimulator_h
#define G4MuDecSimulator_h 1

// C++ libraries
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

// Framework libraries
#include "Particle.h"
#include "Event.h"
#include "Detector.h"
#include "G4MBaseApplication.h"

class G4MuDecSimulator : public G4MBaseApplication
{
  public:
  
    G4MuDecSimulator();
    virtual ~G4MuDecSimulator() {;}

    bool RunSimulation(Event& aEvent) override;

    // static members 
    static Particle currentParticle;
    static G4MuDecSimulator* fG4MuDecSimulator;

    // name of configuration file
    std::string fCfgFile;

};

#endif
