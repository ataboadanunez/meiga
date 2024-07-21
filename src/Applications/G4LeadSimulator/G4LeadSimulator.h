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

class G4RunManager;

class G4LeadSimulator 
{
  public:
  
    G4LeadSimulator();
    virtual ~G4LeadSimulator();

    // main methods of the application
    void Initialize(Event& evt, std::string fileName);
    bool RunSimulation(Event& evt);
    void WriteEventInfo(Event& evt);

    inline void AddBrickEnergyDeposit(const G4double aEnergy) { fBrickTotalEnergyDepositVector.push_back(aEnergy); }
    std::vector<G4double> GetBrickEnergyDeposit() const { return fBrickTotalEnergyDepositVector; }

    // static members 
    static Particle currentParticle;
    
    G4LeadSimulator* fG4LeadSimulator;
    bool fSimulateBrick;
    std::vector<G4double> fBrickTotalEnergyDepositVector;

    // name of configuration file
    std::string fCfgFile;

  private:
    
    G4VisManager* fVisManager = nullptr;
    G4RunManager* fRunManager = nullptr;


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

  	friend class G4LeadDetectorConstructor;
  	friend class G4LeadPrimaryGenerator;
};

#endif
