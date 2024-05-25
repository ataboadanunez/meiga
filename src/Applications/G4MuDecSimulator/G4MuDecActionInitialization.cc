#include "G4MuDecActionInitialization.h"
#include "G4MPrimaryGeneratorAction.h"
#include "G4MuDecStackingAction.h"
#include "G4MuDecRunAction.h"
#include "G4MuDecEventAction.h"
#include "G4MuDecSteppingAction.h"
#include "G4MuDecTrackingAction.h"

G4MuDecActionInitialization::G4MuDecActionInitialization(Event &aEvent) 
  : G4VUserActionInitialization(), fEvent(aEvent)
{
}

void 
G4MuDecActionInitialization::BuildForMaster() const
{
  SetUserAction(new G4MuDecRunAction());
}

void 
G4MuDecActionInitialization::Build() const
{
    SetUserAction(new G4MPrimaryGeneratorAction(fEvent));
    SetUserAction(new G4MuDecTrackingAction(fEvent));
    SetUserAction(new G4MuDecStackingAction(fEvent));
    SetUserAction(new G4MuDecEventAction(fEvent));
    SetUserAction(new G4MuDecSteppingAction());
}
