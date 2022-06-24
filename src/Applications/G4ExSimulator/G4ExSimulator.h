#ifndef G4ExSimulator_h
#define G4ExSimulator_h 1

// C++ libraries
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

// Framework libraries
#include "Particle.h"
#include "Event.h"
#include "SimData.h"
#include "Detector.h"

class G4RunManager;

class G4ExSimulator 
{
	public:
	
		G4ExSimulator();
		virtual ~G4ExSimulator() {;}

		// main methods of the application
		void Initialize(Event& evt, std::string fileName);
		bool RunSimulation(Event& evt);
		void WriteEventInfo(Event& evt);

		// static members 
		static Particle currentParticle;
		static G4ExSimulator* fG4ExSimulator;

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

		friend class G4ExDetectorConstructor;
		friend class G4ExPrimaryGenerator;
};

#endif
