// definition of the G4CasposoSteppingAction class

#ifndef G4CasposoSteppingAction_h
#define G4CasposoSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4CasposoDetectorConstruction;
class G4CasposoEventAction;

/* Stepping action class.
     UserSteppingAction collects info at a step level
*/

class G4CasposoSteppingAction : public G4UserSteppingAction
{
    public:
        G4CasposoSteppingAction(const G4CasposoDetectorConstruction* det, G4CasposoEventAction* event, Event& theEvent);
        virtual ~G4CasposoSteppingAction();
        virtual void UserSteppingAction(const G4Step* step);

    private:
        const G4CasposoDetectorConstruction* fDetectorConstruction;
        G4CasposoEventAction* fEventAction;
        std::ofstream* fOutputFile;
        Event& fEvent;

        G4double fSiPMTime;
        G4double fScinTime;
        G4String stepVolume;
        G4String trackVolume;
};

#endif