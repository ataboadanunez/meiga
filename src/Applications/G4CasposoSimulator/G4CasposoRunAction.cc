// implementation of the G4CasposoRunAction class

#include "G4Timer.hh"
#include "G4Run.hh"
#include "g4root.hh"
#include "G4AccumulableManager.hh"
 
#include "G4CasposoRunAction.h"
//#include "histosRun.hh"

//#include <TFile.h>

G4CasposoRunAction::G4CasposoRunAction()
 : G4UserRunAction()
{
  G4cout << "...G4CasposoRunAction..." << G4endl;
}


G4CasposoRunAction::~G4CasposoRunAction()
{}


void 
G4CasposoRunAction::BeginOfRunAction(const G4Run* )
{
  
}


void 
G4CasposoRunAction::EndOfRunAction(const G4Run* )
{
  
}
