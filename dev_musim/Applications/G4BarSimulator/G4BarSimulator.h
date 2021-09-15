#ifndef G4BarSimulator_h
#define G4BarSimulator_h 1

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

#include "Particle.h"
#include "Event.h"
#include "Detector.h"

class G4RunManager;
class G4StationPrimaryGenerator;

class G4BarSimulator 
{
  public:
  
    G4BarSimulator();
    virtual ~G4BarSimulator() {;}

    void Initialize(Event& evt, std::string fileName);
    bool RunSimulation(Event& evt);
    bool WriteEventInfo(Event& evt);

    static Particle currentParticle;
    static G4BarSimulator* fG4BarSimulator;

    std::string fCfgFile;

    // temporary here
    std::ofstream* fOutput;

  private:



		// cfg flags
		std::string fInputFile;
		std::string fOutputFile; 
		bool fGeoVisOn = true;
		bool fTrajVisOn = false;
		int fVerbosity = 1;
		std::string fRenderFile = "VRML2FILE";

  	friend class G4BarDetectorConstructor;
  	friend class G4BarPrimaryGenerator;
};

#endif
