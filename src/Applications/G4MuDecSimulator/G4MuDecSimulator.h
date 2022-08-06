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

class G4RunManager;

class G4MuDecSimulator 
{
  public:
  
    G4MuDecSimulator();
    virtual ~G4MuDecSimulator() {;}

    // main methods of the application
    void Initialize(Event& evt, std::string fileName);
    bool RunSimulation(Event& evt);
    void WriteEventInfo(Event& evt);

    // static members 
    static Particle currentParticle;
    static G4MuDecSimulator* fG4MuDecSimulator;

    // name of configuration file
    std::string fCfgFile;

  private:

  	friend class G4MuDecDetectorConstructor;
  	friend class G4MuDecPrimaryGenerator;
};

#endif
