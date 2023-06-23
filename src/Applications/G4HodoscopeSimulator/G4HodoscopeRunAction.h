// definition of the G4HodoscopeRunAction class

#ifndef G4HodoscopeRunAction_h
#define G4HodoscopeRunAction_h 1
// Geant4 headers
#include "globals.hh"
#include "G4UserRunAction.hh"
#include "g4root.hh"
// Meiga headers
#include "Event.h"
#include "G4HodoscopeSimulator.h"
// C++ headers
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>


class G4Run;


class G4HodoscopeRunAction : public G4UserRunAction
{
	public:
		G4HodoscopeRunAction(Event& theEvent);
		virtual ~G4HodoscopeRunAction();

		virtual void BeginOfRunAction(const G4Run* aRun);
		virtual void EndOfRunAction(const G4Run* aRun);

	private:

		std::ofstream fOutFile;
		Event& fEvent;


	friend class G4HodoscopeSimulator;

};
#endif 
