#include "G4MuDecEventAction.h"
#include "G4MuDecSimulator.h"
#include "DetectorSimData.h"

#include "G4Track.hh"

G4MuDecEventAction::G4MuDecEventAction(Event& theEvent) : G4UserEventAction(),
	fEvent(theEvent)

{
	
	G4cout << "...G4MuDecEventAction..." << G4endl;

}

G4MuDecEventAction::~G4MuDecEventAction()
{

}

void
G4MuDecEventAction::BeginOfEventAction(const G4Event*)
{

	G4MuDecEventAction::fNumCerenkov = 0;
	G4MuDecEventAction::fNumCerenkovDelta = 0;
	G4MuDecEventAction::fStepLength = 0;
	
	trackLength = 0;
	DetectorSimData& detSimData = fEvent.GetSimData().GetDetectorSimData();  
	auto& muonDecayIDs = detSimData.GetMuonDecayID();
	muonDecayIDs.clear();

}

void 
G4MuDecEventAction::EndOfEventAction(const G4Event* )
{
	
	double particleMomentum = G4MuDecSimulator::currentParticle.GetMomentum();
	int numberCerenkov = G4MuDecEventAction::GetNumberCerenkov();
	int numberCerenkovDelta = G4MuDecEventAction::GetNumberCerenkovDelta();
	trackLength = G4MuDecEventAction::GetTrackLength();
	std::cout << "CerenkovCounter " << particleMomentum / CLHEP::GeV << " " << numberCerenkov << " " << numberCerenkovDelta << " " << trackLength / CLHEP::cm << std::endl;

}

