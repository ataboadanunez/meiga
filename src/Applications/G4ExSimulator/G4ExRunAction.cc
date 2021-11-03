// implementation of the G4ExRunAction class

#include "G4Timer.hh"
#include "G4Run.hh"
#include "g4root.hh"
#include "G4AccumulableManager.hh"
 
#include "G4ExRunAction.h"
//#include "histosRun.hh"

//#include <TFile.h>

G4ExRunAction::G4ExRunAction()
 : G4UserRunAction()
{
  G4cout << "...G4ExRunAction..." << G4endl;
}


G4ExRunAction::~G4ExRunAction()
{}


void 
G4ExRunAction::BeginOfRunAction(const G4Run* )
{
  
}


void 
G4ExRunAction::EndOfRunAction(const G4Run* )
{
  
}
