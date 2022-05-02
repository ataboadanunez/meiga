// implementation of the G4RICHRunAction class

#include "G4Timer.hh"
#include "G4Run.hh"
#include "g4root.hh"
#include "G4AccumulableManager.hh"
 
#include "G4RICHRunAction.h"
//#include "histosRun.hh"

//#include <TFile.h>

G4RICHRunAction::G4RICHRunAction()
 : G4UserRunAction()
{
  G4cout << "...G4RICHRunAction..." << G4endl;
}


G4RICHRunAction::~G4RICHRunAction()
{}


void 
G4RICHRunAction::BeginOfRunAction(const G4Run* )
{
  
}


void 
G4RICHRunAction::EndOfRunAction(const G4Run* )
{
  
}
