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
        G4ExSteppingAction(const G4ExDetectorConstruction* det, G4ExEventAction* event, Event& theEvent);
        virtual ~G4ExSteppingAction();
        virtual void UserSteppingAction(const G4Step* step);

    private:
        const G4ExDetectorConstruction* fDetectorConstruction;
        G4ExEventAction* fEventAction;
        std::ofstream* fOutputFile;
        Event& fEvent;

        G4double fSiPMTime;
        G4double fScinTime;
        G4String stepVolume;
        G4String trackVolume;
};

#endif