#ifndef G4MudulusSimulator_h
#define G4MudulusSimulator_h 1

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
class G4StationPrimaryGenerator;

class G4MudulusSimulator 
{
  public:
  
    G4MudulusSimulator();
    virtual ~G4MudulusSimulator() {;}

    // main methods of the application
    void Initialize(Event& evt, std::string fileName);
    bool RunSimulation(Event& evt);
    void WriteEventInfo(Event& evt);

    // static members 
    static Particle currentParticle;
    static G4MudulusSimulator* fG4MudulusSimulator;

    // name of configuration file
    std::string fCfgFile;

  private:

		// flags for configuration (see .json file)
		std::string fInputFile;
		std::string fOutputFile;
    std::string fDetectorList;
		bool fGeoVisOn = true;
		bool fTrajVisOn = false;
		int fVerbosity = 1;

		std::string fRenderFile = "VRML2FILE";
    std::string fPhysicsName = "QGSP_BERT_HP";
    std::string fSimulationMode;

  	friend class G4MudulusDetectorConstructor;
  	friend class G4MudulusPrimaryGenerator;
};

#endif
