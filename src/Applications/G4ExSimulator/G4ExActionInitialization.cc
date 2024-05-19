#include "G4ExActionInitialization.h"
#include "G4MPrimaryGeneratorAction.h"
#include "G4ExRunAction.h"
#include "G4ExEventAction.h"
#include "G4ExSteppingAction.h"
#include "G4ExTrackingAction.h"

G4ExActionInitialization::G4ExActionInitialization(Event &aEvent) 
  : G4VUserActionInitialization(), fEvent(aEvent)
{
}

void 
G4ExActionInitialization::BuildForMaster() const
{
  SetUserAction(new G4ExRunAction());
}

void 
G4ExActionInitialization::Build() const
{
    SetUserAction(new G4MPrimaryGeneratorAction(fEvent));
    SetUserAction(new G4ExEventAction());
    SetUserAction(new G4ExSteppingAction());
}
