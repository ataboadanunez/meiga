// definition of the G4CasposoRunAction class

#ifndef G4CasposoRunAction_h
#define G4CasposoRunAction_h 1
// Geant4 headers
#include "globals.hh"
#include "G4UserRunAction.hh"
#include "g4root.hh"
// Meiga headers
#include "Event.h"
#include "G4CasposoSimulator.h"
// C++ headers
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>


class G4Run;


class G4CasposoRunAction : public G4UserRunAction
{
	public:
		G4CasposoRunAction(Event& theEvent);
		virtual ~G4CasposoRunAction();

		virtual void BeginOfRunAction(const G4Run* aRun);
		virtual void EndOfRunAction(const G4Run* aRun);

	private:

		std::ofstream* fOutFile;
		Event& fEvent;


	friend class G4CasposoSimulator;

};
#endif 
