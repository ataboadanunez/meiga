// definition of the G4ExSteppingAction class

#ifndef G4ExSteppingAction_h
#define G4ExSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4ExDetectorConstruction;
class G4ExEventAction;

/* Stepping action class.
     UserSteppingAction collects info at a step level
*/

class G4ExSteppingAction : public G4UserSteppingAction
{
    public:
        G4ExSteppingAction();
        virtual ~G4ExSteppingAction();
        virtual void SetSteppingManagerPointer(G4SteppingManager* pValue);
        virtual void UserSteppingAction(const G4Step* step);

};

#endif