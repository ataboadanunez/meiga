#include "G4MuDecTrackingAction.h"
#include "G4MuDecSimulator.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4TrackStatus.hh>
#include <G4OpticalPhoton.hh>

#include "Particle.h"


G4MuDecTrackingAction::G4MuDecTrackingAction(G4MuDecEventAction* G4event, const bool countCerenkov) : 
	fEventAction(G4event),
	fCountCerenkov(countCerenkov)
	{

		if (fCountCerenkov)
			std::cout << "[INFO] G4MuDecTrackingAction: Counting Cerenkov photons." << std::endl;

	}


void
G4MuDecTrackingAction::PreUserTrackingAction(const G4Track* track)
{ 
	
	const G4ParticleDefinition* particle = track->GetParticleDefinition();
	const int parentId = track->GetParentID();
	const int trackId = track->GetTrackID();

	// handle optical photons
	if (fCountCerenkov && (particle == G4OpticalPhoton::OpticalPhotonDefinition())) {
		// check if photons are within energy range
		double phEnergy = track->GetTotalEnergy() / CLHEP::eV;
		double phWaveLength = 1240 / phEnergy;
		if ((300 <= phWaveLength) && (phWaveLength <= 570)) {
			if (parentId != 1)
				fEventAction->AddCerenkovDelta();
			else 
				fEventAction->AddCerenkov();
		}	
	}
	else
		return;

	// check 
	bool isMuonDecay = false;

	// get physical volume name
	G4String physVolName = track->GetVolume()->GetName();

	if ( (particle->GetParticleName() == "e-" || particle->GetParticleName() == "e+") && parentId == 1) {
		const auto* process = track->GetCreatorProcess();
		if (process->GetProcessType() == fDecay) {
			// std::cout << "[DEBUG] G4MuDecSteppingAction: This " << particle->GetParticleName() << " (" << trackId << ") was produced via " << process->GetProcessName() << " from parent ID " << parentId << std::endl;
			double electronEnergy = track->GetTotalEnergy();
			//std::cout << "Volume " << physVolName << " Michel_Electron " << trackId << " Energy " << electronEnergy / CLHEP::MeV << std::endl; 
			//muonDecayIDs.insert(trackId);
		
		}
	}

}	 

void
G4MuDecTrackingAction::PostUserTrackingAction(const G4Track* track)
{
	firstStepInVolume = false;
	Particle& currParticle = G4MuDecSimulator::currentParticle;
	int particleId = currParticle.GetParticleId();
	double particleEnergy = currParticle.GetTotalEnergy();
	double particleZenith = currParticle.GetZenith();
	G4ThreeVector trackPosition = track->GetPosition();
	std::vector<double> injectPosition = currParticle.GetInjectionPosition();
	double trackZ = trackPosition[2] / CLHEP::m;
	double injectZ = injectPosition[2] / CLHEP::m;

	double depth = injectZ - trackZ;

	// track primaries
	//if (track->GetParentID() == 0) {
		//G4cout << "TrackStoppingInfo " << particleId << " " << particleEnergy / CLHEP::MeV << " " << particleZenith / CLHEP::deg << " " << depth << " " << track->GetVolume()->GetName() << G4endl; 
	//}
}
