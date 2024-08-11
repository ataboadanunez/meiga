#ifndef G4LeadSimulator_h
#define G4LeadSimulator_h 1
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
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

class G4LeadSimulator : public G4MBaseApplication
{
  public:
  
    G4LeadSimulator();
    virtual ~G4LeadSimulator() {;}

    void Initialize(Event &aEvent, std::string aFileName) override;
    bool RunSimulation(Event &aEvent) override;

    // static members 
    static Particle currentParticle;
    static G4LeadSimulator* fG4LeadSimulator;
  
  private:
    // name of configuration file
    std::string fCfgFile;
    bool fSimulateBrick;
};

#endif
