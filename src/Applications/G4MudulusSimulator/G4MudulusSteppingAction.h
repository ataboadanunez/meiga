// definition of the G4MudulusSteppingAction class

#ifndef G4MudulusSteppingAction_h
#define G4MudulusSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4MudulusDetectorConstruction;
class G4MudulusEventAction;

/* Stepping action class.
     UserSteppingAction collects info at a step level
*/

class G4MudulusSteppingAction : public G4UserSteppingAction
{
    public:
        G4MudulusSteppingAction(const G4MudulusDetectorConstruction* det, G4MudulusEventAction* event, Event& theEvent);
        virtual ~G4MudulusSteppingAction();
        virtual void UserSteppingAction(const G4Step* step);

    private:
        const G4MudulusDetectorConstruction* fDetectorConstruction;
        G4MudulusEventAction* fEventAction;
        std::ofstream* fOutputFile;
        Event& fEvent;

        G4double fSiPMTime;
        G4double fScinTime;
        G4String stepVolume;
        G4String trackVolume;
};

#endif