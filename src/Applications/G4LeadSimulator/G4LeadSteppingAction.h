// definition of the G4LeadSteppingAction class

#ifndef G4LeadSteppingAction_h
#define G4LeadSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4LeadDetectorConstruction;
class G4LeadEventAction;

/* Stepping action class.
	 UserSteppingAction collects info at a step level
*/

class G4LeadSteppingAction : public G4UserSteppingAction
{
	public:
		G4LeadSteppingAction(Event &aEvent);
		virtual ~G4LeadSteppingAction();
		virtual void UserSteppingAction(const G4Step* step);

	private:
		Event& fEvent;
};

#endif