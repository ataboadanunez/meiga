// definition of the G4NeutronSteppingAction class

#ifndef G4NeutronSteppingAction_h
#define G4NeutronSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4NeutronDetectorConstruction;
class G4NeutronEventAction;

/* Stepping action class.
	 UserSteppingAction collects info at a step level
*/

class G4NeutronSteppingAction : public G4UserSteppingAction
{
	public:
		G4NeutronSteppingAction(const G4NeutronDetectorConstruction* det, G4NeutronEventAction* event, Event& theEvent);
		virtual ~G4NeutronSteppingAction();
		virtual void UserSteppingAction(const G4Step* step);

	private:
		const G4NeutronDetectorConstruction* fDetectorConstruction;
		G4NeutronEventAction* fEventAction;
		std::ofstream* fOutputFile;
		Event& fEvent;
};

#endif