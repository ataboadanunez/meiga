// implementation of the G4NeutronRunAction class

#include "G4Timer.hh"
#include "G4Run.hh"
// #include "g4root.hh"
#include "G4AccumulableManager.hh"
 
#include "G4NeutronRunAction.h"
//#include "histosRun.hh"

//#include <TFile.h>

G4NeutronRunAction::G4NeutronRunAction()
 : G4UserRunAction()
{
  G4cout << "...G4NeutronRunAction..." << G4endl;
}


G4NeutronRunAction::~G4NeutronRunAction()
{}


void 
G4NeutronRunAction::BeginOfRunAction(const G4Run* )
{
  
}


void 
G4NeutronRunAction::EndOfRunAction(const G4Run* )
{
  
}
