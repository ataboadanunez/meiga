// definition of the G4MuDecSteppingAction class

#ifndef G4MuDecSteppingAction_h
#define G4MuDecSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4MuDecConstruction;
class G4MuDecEventAction;


class G4MuDecSteppingAction : public G4UserSteppingAction
{
	public:
		G4MuDecSteppingAction(const G4MuDecConstruction* det, G4MuDecEventAction* event, Event& theEvent);
			virtual ~G4MuDecSteppingAction();
			virtual void UserSteppingAction(const G4Step* step);

	private:
		const G4MuDecConstruction* fDetectorConstruction;
		G4MuDecEventAction* fEventAction;
		std::ofstream* fOutputFile;
		Event& fEvent;

};

#endif