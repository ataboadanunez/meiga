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
    void Initialize(Event& evt, std::string fileName);
    bool RunSimulation(Event& evt);
    void WriteEventInfo(Event& evt);

    // static members 
    static Particle currentParticle;
    static G4HodoscopeSimulator* fG4HodoscopeSimulator;

    // name of configuration file
    std::string fCfgFile;

  private:

		// flags for configuration (see .json file)
    std::string fInputFile;
    std::string fOutputFile;
    std::string fDetectorList;
    std::string fDetectorProperties;
    SimData::SimulationMode fSimulationMode;
    SimData::InjectionMode fInjectionMode;

    bool fGeoVisOn = true;
    bool fTrajVisOn = false;
    int fVerbosity = 1;
    std::string fRenderFile = "VRML2FILE";
    std::string fPhysicsName = "QGSP_BERT_HP";

  	friend class G4HodoscopeDetectorConstructor;
  	friend class G4HodoscopePrimaryGenerator;
};

#endif
