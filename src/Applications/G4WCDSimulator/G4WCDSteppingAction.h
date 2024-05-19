// definition of the G4WCDSteppingAction class

#ifndef G4WCDSteppingAction_h
#define G4WCDSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4WCDConstruction;
class G4WCDEventAction;

/* Stepping action class.
     UserSteppingAction collects info at a step level
*/

class G4WCDSteppingAction : public G4UserSteppingAction
{
    public:
        G4WCDSteppingAction();
        virtual ~G4WCDSteppingAction();
        virtual void UserSteppingAction(const G4Step* step);
};

#endif