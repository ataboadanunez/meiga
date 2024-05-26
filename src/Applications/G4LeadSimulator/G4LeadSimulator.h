#ifndef G4LeadSimulator_h
#define G4LeadSimulator_h 1

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

class G4LeadSimulator 
{
  public:
  
    G4LeadSimulator();
    virtual ~G4LeadSimulator() {;}

    // main methods of the application
    void Initialize(Event &aEvent, std::string aFileName);
    bool RunSimulation(Event &aEvent);
    void WriteEventInfo(Event &aEvent);

    // static members 
    static Particle currentParticle;
    static G4LeadSimulator* fG4LeadSimulator;
  
  private:
    // name of configuration file
    std::string fCfgFile;
};

#endif
