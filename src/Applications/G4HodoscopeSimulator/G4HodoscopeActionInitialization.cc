#include "G4HodoscopeActionInitialization.h"
#include "G4MPrimaryGeneratorAction.h"
#include "G4HodoscopeRunAction.h"
#include "G4HodoscopeEventAction.h"
#include "G4HodoscopeSteppingAction.h"
#include "G4HodoscopeTrackingAction.h"

G4HodoscopeActionInitialization::G4HodoscopeActionInitialization(Event &aEvent) 
  : G4VUserActionInitialization(), fEvent(aEvent)
{
}

void 
G4HodoscopeActionInitialization::BuildForMaster() const
{
}

void 
G4HodoscopeActionInitialization::Build() const
{
    SetUserAction(new G4MPrimaryGeneratorAction(fEvent));
    SetUserAction(new G4HodoscopeRunAction(fEvent));
    SetUserAction(new G4HodoscopeEventAction(fEvent));
    SetUserAction(new G4HodoscopeSteppingAction(fEvent));
}
