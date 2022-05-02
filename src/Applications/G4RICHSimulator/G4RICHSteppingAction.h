// definition of the G4RICHSteppingAction class

#ifndef G4RICHSteppingAction_h
#define G4RICHSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4RICHConstruction;
class G4RICHEventAction;

/* Stepping action class.
     UserSteppingAction collects info at a step level
*/

class G4RICHSteppingAction : public G4UserSteppingAction
{
    public:
        G4RICHSteppingAction(const G4RICHConstruction* det, G4RICHEventAction* event, Event& theEvent);
        virtual ~G4RICHSteppingAction();
        virtual void UserSteppingAction(const G4Step* step);

    private:
        const G4RICHConstruction* fDetectorConstruction;
        G4RICHEventAction* fEventAction;
        std::ofstream* fOutputFile;
        Event& fEvent;

        G4double fSiPMTime;
        G4double fScinTime;
        G4String stepVolume;
        G4String trackVolume;
};

#endif