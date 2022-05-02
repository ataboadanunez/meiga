#include "G4RICHTrackingAction.h"
#include "G4RICHSimulator.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4TrackStatus.hh>
#include <G4OpticalPhoton.hh>

#include "Particle.h"


void
G4RICHTrackingAction::PreUserTrackingAction(const G4Track* track)
{ 
	
	const G4ParticleDefinition* particle = track->GetParticleDefinition();

	// reject photons
		if (particle == G4OpticalPhoton::OpticalPhotonDefinition())
			return;

		int trackId = track->GetTrackID();
		int parentId = track->GetParentID();
		bool isMuonDecay = false;

		// get physical volume name
		G4String physVolName = track->GetVolume()->GetName();

		if ( (particle->GetParticleName() == "e-" || particle->GetParticleName() == "e+") && parentId == 1) {
			const auto* process = track->GetCreatorProcess();
			if (process->GetProcessType() == fDecay) {
				// std::cout << "[DEBUG] G4RICHSteppingAction: This " << particle->GetParticleName() << " (" << trackId << ") was produced via " << process->GetProcessName() << " from parent ID " << parentId << std::endl;
				double electronEnergy = track->GetTotalEnergy();
				//std::cout << "Volume " << physVolName << " Michel_Electron " << trackId << " Energy " << electronEnergy / CLHEP::MeV << std::endl; 
				//muonDecayIDs.insert(trackId);
			
			}
		}
	 
}


void
G4RICHTrackingAction::PostUserTrackingAction(const G4Track* track)
{
	firstStepInVolume = false;
	Particle& currParticle = G4RICHSimulator::currentParticle;
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
