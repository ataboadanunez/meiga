// implementation of the G4CasposoEventAction class

#include "G4CasposoEventAction.h"

#include "G4Track.hh"

G4CasposoEventAction::G4CasposoEventAction() : G4UserEventAction()
{
  G4cout << "...G4CasposoEventAction..." << G4endl;

}

G4CasposoEventAction::~G4CasposoEventAction()
{

}

void
G4CasposoEventAction::BeginOfEventAction(const G4Event*)
{

}

void 
G4CasposoEventAction::EndOfEventAction(const G4Event* )
{
  
}

