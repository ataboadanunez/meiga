// definition of the G4TandarRunAction class

#ifndef G4TandarRunAction_h
#define G4TandarRunAction_h 1
// Geant4 headers
#include "globals.hh"
#include "G4UserRunAction.hh"
// #include "g4root.hh"
// Meiga headers
#include "Event.h"
#include "G4TandarSimulator.h"
// C++ headers
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>


class G4Run;


class G4TandarRunAction : public G4UserRunAction
{
	public:
		G4TandarRunAction(Event& theEvent);
		virtual ~G4TandarRunAction();

		virtual void BeginOfRunAction(const G4Run* aRun);
		virtual void EndOfRunAction(const G4Run* aRun);

	private:

		std::ofstream fOutFile;
		Event& fEvent;


	friend class G4TandarSimulator;

};
#endif 
