#ifndef G4WCDSimulator_h
#define G4WCDSimulator_h 1

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

class G4WCDSimulator : public G4MBaseApplication
{
	public:
	
		G4WCDSimulator();
		virtual ~G4WCDSimulator() {;}

		bool RunSimulation(Event& aEvent) override;

		// static members 
		static Particle currentParticle;
		static G4WCDSimulator* fG4WCDSimulator;

		// name of configuration file
		std::string fCfgFile;

};

#endif
