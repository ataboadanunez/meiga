// implementation of the G4WCDEventAction class

#include "G4WCDEventAction.h"

#include "G4Track.hh"

G4WCDEventAction::G4WCDEventAction() : G4UserEventAction()
{
  G4cout << "...G4WCDEventAction..." << G4endl;

}

G4WCDEventAction::~G4WCDEventAction()
{

}

void
G4WCDEventAction::BeginOfEventAction(const G4Event*)
{

}

void 
G4WCDEventAction::EndOfEventAction(const G4Event* )
{
  
}

