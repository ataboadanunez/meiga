// implementation of the G4RockSteppingAction class
#include "G4RockSimulator.h"
#include "G4RockSteppingAction.h"
#include "G4RockDetectorConstruction.h"
#include "G4RockEventAction.h"
#include "G4RockTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"

#include "Particle.h"
#include "SimData.h"
#include "SiPMSimData.h"
#include "Detector.h"
#include "Module.h"

G4RockSteppingAction::G4RockSteppingAction(const G4RockDetectorConstruction* det, G4RockEventAction* G4event, Event& theEvent)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
    fEvent(theEvent)
{
}

G4RockSteppingAction::~G4RockSteppingAction()
{
}

void
G4RockSteppingAction::UserSteppingAction(const G4Step* step)
{
  
	// keep only primaries and muons (also from decay)
	auto track = step->GetTrack();
	const int particleId = track->GetDefinition()->GetPDGEncoding();
	G4RockTrackingAction* const trackingA = (G4RockTrackingAction*)G4RunManager::GetRunManager()->GetUserTrackingAction();



	if (!(((particleId == Particle::eMuon) || (particleId == Particle::eAntiMuon)) || (track->GetParentID() == 0))){
		G4cout << "[DEBUG] G4RockSteppingAction: Secondary Particle " << track->GetParticleDefinition()->GetParticleName() << " was killed! " << G4endl;
		track->SetTrackStatus(fStopAndKill);
	}

	// if ((track->GetParentID() == 0) || ( (particleId == Particle::eMuon) || (particleId == Particle::eAntiMuon) ) ) {
		
	// 	// if not primary it means muon comes from decay
	// 	if (track->GetParentID() != 0) {
	// 		trackingA->SetIsMuonDecay();
	// 		G4cout << "[DEBUG] G4RockSteppingAction: Secondary particle " << track->GetParticleDefinition()->GetParticleName() << " is from decay! " << G4endl;
	// 	}
		
	// }
	// else {
	// 	G4cout << "[DEBUG] G4RockSteppingAction: Secondary Particle " << track->GetParticleDefinition()->GetParticleName() << " was killed! " << G4endl;
	// 	track->SetTrackStatus(fStopAndKill);
	// }

}
