#include "G4LeadActionInitialization.h"
#include "G4MPrimaryGeneratorAction.h"
#include "G4LeadRunAction.h"
#include "G4LeadEventAction.h"
#include "G4LeadSteppingAction.h"
#include "G4LeadTrackingAction.h"

G4LeadActionInitialization::G4LeadActionInitialization(Event &aEvent) 
  : G4VUserActionInitialization(), fEvent(aEvent)
{
}

void 
G4LeadActionInitialization::BuildForMaster() const
{
}

void 
G4LeadActionInitialization::Build() const
{
    SetUserAction(new G4MPrimaryGeneratorAction(fEvent));
    SetUserAction(new G4LeadRunAction(fEvent));
    SetUserAction(new G4LeadEventAction(fEvent));
    SetUserAction(new G4LeadSteppingAction(fEvent));
}
