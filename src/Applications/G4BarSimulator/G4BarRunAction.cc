#include "G4Timer.hh"
#include "G4Run.hh"
#include "g4root.hh"
#include "G4AccumulableManager.hh"

#include "G4BarRunAction.h"


G4BarRunAction::G4BarRunAction()
 : G4UserRunAction()
{
  G4cout << "...G4BarRunAction..." << G4endl;
}


G4BarRunAction::~G4BarRunAction()
{}


void 
G4BarRunAction::BeginOfRunAction(const G4Run* )
{
  
}


void 
G4BarRunAction::EndOfRunAction(const G4Run* )
{
  
}
