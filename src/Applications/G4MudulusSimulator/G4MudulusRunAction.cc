// implementation of the G4MudulusRunAction class

#include "G4Timer.hh"
#include "G4Run.hh"
#include "g4root.hh"
#include "G4AccumulableManager.hh"
 
#include "G4MudulusRunAction.h"
//#include "histosRun.hh"

//#include <TFile.h>

G4MudulusRunAction::G4MudulusRunAction()
 : G4UserRunAction()
{
  G4cout << "...G4MudulusRunAction..." << G4endl;
}


G4MudulusRunAction::~G4MudulusRunAction()
{}


void 
G4MudulusRunAction::BeginOfRunAction(const G4Run* )
{
  
}


void 
G4MudulusRunAction::EndOfRunAction(const G4Run* )
{
  
}
