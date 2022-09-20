// definition of the G4RockSteppingAction class

#ifndef G4RockSteppingAction_h
#define G4RockSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4RockDetectorConstruction;
class G4RockEventAction;

/* Stepping action class.
     UserSteppingAction collects info at a step level
*/

class G4RockSteppingAction : public G4UserSteppingAction
{
    public:
        G4RockSteppingAction(const G4RockDetectorConstruction* det, G4RockEventAction* event, Event& theEvent);
        virtual ~G4RockSteppingAction();
        virtual void UserSteppingAction(const G4Step* step);

    private:
        const G4RockDetectorConstruction* fDetectorConstruction;
        G4RockEventAction* fEventAction;
        std::ofstream* fOutputFile;
        Event& fEvent;

        G4double fSiPMTime;
        G4double fScinTime;
        G4String stepVolume;
        G4String trackVolume;
};

#endif