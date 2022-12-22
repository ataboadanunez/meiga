// definition of the G4AndesSteppingAction class

#ifndef G4AndesSteppingAction_h
#define G4AndesSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4AndesDetectorConstruction;
class G4AndesEventAction;

/* Stepping action class.
	 UserSteppingAction collects info at a step level
*/

class G4AndesSteppingAction : public G4UserSteppingAction
{
	public:
		G4AndesSteppingAction(const G4AndesDetectorConstruction* det, G4AndesEventAction* event, Event& theEvent);
		virtual ~G4AndesSteppingAction();
		virtual void UserSteppingAction(const G4Step* step);

	private:
		const G4AndesDetectorConstruction* fDetectorConstruction;
		G4AndesEventAction* fEventAction;
		std::ofstream* fOutputFile;
		Event& fEvent;

};

#endif