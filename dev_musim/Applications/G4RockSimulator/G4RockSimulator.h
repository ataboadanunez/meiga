// definition of the G4RockSimulator class
#ifndef G4RockSimulator_h
#define G4RockSimulator_h 1

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

#include "Event.h"
#include "Particle.h"
#include "Detector.h"

class G4RunManager;
class G4StationPrimaryGenerator;

class G4RockSimulator 
{
	public:
		G4RockSimulator();
		virtual ~G4RockSimulator() {;}

		// main methods of the application
		void Initialize(Event& evt, std::string fileName);
		bool RunSimulation(Event& evt);
		void WriteEventInfo(Event& evt);

		static Particle currentParticle;
		static G4RockSimulator* fG4RockSimulator;

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

		friend class G4RockDetectorConstructor;
		friend class G4RockPrimaryGenerator;

};

#endif
