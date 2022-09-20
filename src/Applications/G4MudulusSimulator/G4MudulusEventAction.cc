// implementation of the G4MudulusEventAction class

#include "G4MudulusEventAction.h"

#include "G4Track.hh"

G4MudulusEventAction::G4MudulusEventAction() : G4UserEventAction()
{
  G4cout << "...G4MudulusEventAction..." << G4endl;

}

G4MudulusEventAction::~G4MudulusEventAction()
{

}

void
G4MudulusEventAction::BeginOfEventAction(const G4Event*)
{

}

void 
G4MudulusEventAction::EndOfEventAction(const G4Event* )
{
  
}

