// implementation of the G4RockActionInitialization class

#include "G4RockActionInitialization.h"
#include "G4RockPrimaryGeneratorAction.h"
#include "G4RockDetectorConstruction.h"
#include "G4RockRunAction.h"
#include "G4RockEventAction.h"
#include "G4RockSteppingAction.h"


G4RockActionInitialization::G4RockActionInitialization(const G4RockDetectorConstruction* det)
 : G4VUserActionInitialization(),
   fDetectorConstruction(det)
{
	//fParticleInjector = new ParticleInjector();
}

G4RockActionInitialization::~G4RockActionInitialization()
{}
// for now we just leave it like this
/*
void G4RockActionInitialization::BuildForMaster() const
{
  G4RockRunAction* runAction = new G4RockRunAction;
  SetUserAction(runAction);
}

*/
// for now we just leave it like this. only Primary Generator Action
void 
G4RockActionInitialization::Build() const
{

  G4RockRunAction* runAction = new G4RockRunAction();
  SetUserAction(runAction);
  
  G4RockPrimaryGeneratorAction* primaryGenAction = new G4RockPrimaryGeneratorAction(/*fParticleInjector*/);
  SetUserAction(primaryGenAction);
  
  
  G4RockEventAction* eventAction = new G4RockEventAction(runAction, primaryGenAction);
  SetUserAction(eventAction);
  
  G4RockSteppingAction* stepAction = new G4RockSteppingAction(fDetectorConstruction, eventAction);
  SetUserAction(stepAction);
  
}  

