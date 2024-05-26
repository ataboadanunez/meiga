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

class G4RunManager;

class G4WCDSimulator 
{
	public:
	
		G4WCDSimulator();
		virtual ~G4WCDSimulator() {;}

		// main methods of the application
		void Initialize(Event& aEvent, std::string aFileName);
		bool RunSimulation(Event& aEvent);
		void WriteEventInfo(Event& aEvent);

		// static members 
		static Particle currentParticle;
		static G4WCDSimulator* fG4WCDSimulator;

		// name of configuration file
		std::string fCfgFile;

};

#endif
