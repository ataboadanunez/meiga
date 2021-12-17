#ifndef G4CasposoSimulator_h
#define G4CasposoSimulator_h 1

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

class G4CasposoSimulator 
{
  public:
  
    G4CasposoSimulator();
    virtual ~G4CasposoSimulator() {;}

    // main methods of the application
    void Initialize(Event& evt, std::string fileName);
    bool RunSimulation(Event& evt);
    void WriteEventInfo(Event& evt);

    // static members 
    static Particle currentParticle;
    static G4CasposoSimulator* fG4CasposoSimulator;

    // name of configuration file
    std::string fCfgFile;

  private:

		// flags for configuration (see .json file)
		std::string fInputFile;
		std::string fOutputFile; 
		bool fGeoVisOn = true;
		bool fTrajVisOn = false;
		int fVerbosity = 1;
		std::string fRenderFile = "VRML2FILE";
		std::string fPhysicsName = "QGSP_BERT_HP";
  	friend class G4CasposoDetectorConstructor;
  	friend class G4CasposoPrimaryGenerator;
};

#endif
