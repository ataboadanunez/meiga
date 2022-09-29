// definition of the G4AndesRunAction class

#ifndef G4AndesRunAction_h
#define G4AndesRunAction_h 1
// Geant4 headers
#include "globals.hh"
#include "G4UserRunAction.hh"
#include "g4root.hh"
// Meiga headers
#include "Event.h"
#include "G4AndesSimulator.h"
// C++ headers
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>


class G4Run;


class G4AndesRunAction : public G4UserRunAction
{
	public:
		G4AndesRunAction(Event& theEvent);
		virtual ~G4AndesRunAction();

		virtual void BeginOfRunAction(const G4Run* aRun);
		virtual void EndOfRunAction(const G4Run* aRun);

	private:

		std::ofstream* fOutFile;
		Event& fEvent;


	friend class G4AndesSimulator;

};
#endif 
