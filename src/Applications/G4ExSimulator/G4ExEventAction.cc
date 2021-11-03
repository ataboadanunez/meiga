// implementation of the G4ExEventAction class

#include "G4ExEventAction.h"

#include "G4Track.hh"

G4ExEventAction::G4ExEventAction() : G4UserEventAction()
{
  G4cout << "...G4ExEventAction..." << G4endl;

}

G4ExEventAction::~G4ExEventAction()
{

}

void
G4ExEventAction::BeginOfEventAction(const G4Event*)
{

}

void 
G4ExEventAction::EndOfEventAction(const G4Event* )
{
  
}

