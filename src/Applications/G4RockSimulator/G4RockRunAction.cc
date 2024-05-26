// implementation of the G4RockRunAction class

#include "G4Timer.hh"
#include "G4Run.hh"
// #include "g4root.hh"
#include "G4AccumulableManager.hh"
 
#include "G4RockRunAction.h"
//#include "histosRun.hh"

//#include <TFile.h>

G4RockRunAction::G4RockRunAction()
 : G4UserRunAction()
{
  G4cout << "...G4RockRunAction..." << G4endl;
}


G4RockRunAction::~G4RockRunAction()
{}


void 
G4RockRunAction::BeginOfRunAction(const G4Run* )
{
  
}


void 
G4RockRunAction::EndOfRunAction(const G4Run* )
{
  
}
