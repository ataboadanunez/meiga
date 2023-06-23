// definition of the G4HodoscopeSteppingAction class

#ifndef G4HodoscopeSteppingAction_h
#define G4HodoscopeSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4HodoscopeDetectorConstruction;
class G4HodoscopeEventAction;

/* Stepping action class.
	 UserSteppingAction collects info at a step level
*/

class G4HodoscopeSteppingAction : public G4UserSteppingAction
{
	public:
		G4HodoscopeSteppingAction(G4HodoscopeEventAction* event, Event& theEvent);
		virtual ~G4HodoscopeSteppingAction();
		virtual void UserSteppingAction(const G4Step* step);

	private:
		G4HodoscopeEventAction* fEventAction;
		Event& fEvent;

		//std::vector<int> fBarsX1;
		//std::vector<int> fBarsY1;
};

#endif