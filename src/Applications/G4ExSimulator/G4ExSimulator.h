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
#include "G4MBaseApplication.h"

class G4ExSimulator : public G4MBaseApplication
{
	public:
	
		G4ExSimulator();
		virtual ~G4ExSimulator() {;}

		bool RunSimulation(Event &aEvent) override;

		// static members 
		static Particle currentParticle;
		static G4ExSimulator* fG4ExSimulator;

		// name of configuration file
		std::string fCfgFile;

};

#endif
