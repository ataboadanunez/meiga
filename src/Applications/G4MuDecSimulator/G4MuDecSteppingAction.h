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
		G4MuDecSteppingAction();
			virtual ~G4MuDecSteppingAction();
			virtual void UserSteppingAction(const G4Step* step);

	private:

};

#endif