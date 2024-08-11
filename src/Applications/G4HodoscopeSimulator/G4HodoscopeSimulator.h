#ifndef G4HodoscopeSimulator_h
#define G4HodoscopeSimulator_h 1

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

class G4HodoscopeSimulator : public G4MBaseApplication
{
  public:
  
    G4HodoscopeSimulator();
    virtual ~G4HodoscopeSimulator() {;}

    // main methods of the application
    bool RunSimulation(Event &aEvent) override;

    // static members 
    static Particle currentParticle;
    static G4HodoscopeSimulator* fG4HodoscopeSimulator;

  private:
    // name of configuration file
    std::string fCfgFile;
};

#endif
