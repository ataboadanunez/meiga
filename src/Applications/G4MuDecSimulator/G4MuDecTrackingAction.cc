#include "G4MuDecTrackingAction.h"
#include "G4MuDecSimulator.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4TrackStatus.hh>
#include <G4OpticalPhoton.hh>

#include "Particle.h"


G4MuDecTrackingAction::G4MuDecTrackingAction(Event& aEvent) : 
	fEvent(aEvent)
{
}


void
G4MuDecTrackingAction::PreUserTrackingAction(const G4Track* track)
{ 
	
	// Identify electrons from muon decay inside water
	DetectorSimData& detSimData = fEvent.GetSimData().GetDetectorSimData();
	auto& muonDecayIDs = detSimData.GetMuonDecayID();

	const G4ParticleDefinition* particle = track->GetParticleDefinition();
	const int parentId = track->GetParentID();
	const int trackId = track->GetTrackID();

	// get physical volume name
	G4String physVolName = track->GetVolume()->GetName();

	if ( (particle->GetParticleName() == "e-" || particle->GetParticleName() == "e+") && (parentId == 1) && (physVolName == "physTank")) {
		const auto* process = track->GetCreatorProcess();
		if (process->GetProcessType() == fDecay) {
			muonDecayIDs.insert(trackId);
		}
	}
	
	return;

}	 

void
G4MuDecTrackingAction::PostUserTrackingAction(const G4Track* track)
{
}
