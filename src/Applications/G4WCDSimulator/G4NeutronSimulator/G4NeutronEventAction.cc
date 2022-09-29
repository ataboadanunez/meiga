// implementation of the G4NeutronEventAction class

#include "G4NeutronEventAction.h"

#include "G4Track.hh"

G4NeutronEventAction::G4NeutronEventAction() : G4UserEventAction()
{
  G4cout << "...G4NeutronEventAction..." << G4endl;

}

G4NeutronEventAction::~G4NeutronEventAction()
{

}

void
G4NeutronEventAction::BeginOfEventAction(const G4Event*)
{

}

void 
G4NeutronEventAction::EndOfEventAction(const G4Event* )
{
  
}

