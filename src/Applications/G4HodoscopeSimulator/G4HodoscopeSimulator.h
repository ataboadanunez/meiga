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

class G4RunManager;

class G4HodoscopeSimulator 
{
  public:
  
    G4HodoscopeSimulator();
    virtual ~G4HodoscopeSimulator() {;}

    // main methods of the application
    void Initialize(Event &aEvent, std::string aFileName);
    bool RunSimulation(Event &aEvent);
    void WriteEventInfo(Event &aEvent);

    // static members 
    static Particle currentParticle;
    static G4HodoscopeSimulator* fG4HodoscopeSimulator;

  private:
    // name of configuration file
    std::string fCfgFile;
};

#endif
