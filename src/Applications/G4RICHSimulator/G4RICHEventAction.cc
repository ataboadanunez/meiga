#include "G4RICHEventAction.h"
#include "DetectorSimData.h"

#include "G4Track.hh"

G4RICHEventAction::G4RICHEventAction(Event& theEvent) : G4UserEventAction(),
  fEvent(theEvent)

{
  
  G4cout << "...G4RICHEventAction..." << G4endl;

}

G4RICHEventAction::~G4RICHEventAction()
{

}

void
G4RICHEventAction::BeginOfEventAction(const G4Event*)
{

  DetectorSimData& detSimData = fEvent.GetSimData().GetDetectorSimData();  
  auto& muonDecayIDs = detSimData.GetMuonDecayID();
  muonDecayIDs.clear();

}

void 
G4RICHEventAction::EndOfEventAction(const G4Event* )
{
  

}

