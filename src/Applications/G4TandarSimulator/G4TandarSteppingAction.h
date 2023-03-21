// definition of the G4TandarSteppingAction class

#ifndef G4TandarSteppingAction_h
#define G4TandarSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4TandarDetectorConstruction;
class G4TandarEventAction;

/* Stepping action class.
     UserSteppingAction collects info at a step level
*/

class G4TandarSteppingAction : public G4UserSteppingAction
{
    public:
        G4TandarSteppingAction(const G4TandarDetectorConstruction* det, G4TandarEventAction* event, Event& theEvent);
        virtual ~G4TandarSteppingAction();
        virtual void UserSteppingAction(const G4Step* step);

    private:
        const G4TandarDetectorConstruction* fDetectorConstruction;
        G4TandarEventAction* fEventAction;
        std::ofstream* fOutputFile;
        Event& fEvent;

        G4double fSiPMTime;
        G4double fScinTime;
        G4String stepVolume;
        G4String trackVolume;
};

#endif