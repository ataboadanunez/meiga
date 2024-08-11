// definition of the G4LeadRunAction class

#ifndef G4LeadRunAction_h
#define G4LeadRunAction_h 1
// Geant4 headers
#include "globals.hh"
#include "G4UserRunAction.hh"
// #include "g4root.hh"
// Meiga headers
#include "Event.h"
#include "G4LeadSimulator.h"
// C++ headers
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>


class G4Run;


class G4LeadRunAction : public G4UserRunAction
{
	public:
		G4LeadRunAction(Event& theEvent);
		virtual ~G4LeadRunAction();

		virtual void BeginOfRunAction(const G4Run* aRun);
		virtual void EndOfRunAction(const G4Run* aRun);

	private:

		std::ofstream fOutFile;
		Event& fEvent;


	friend class G4LeadSimulator;

};
#endif 
