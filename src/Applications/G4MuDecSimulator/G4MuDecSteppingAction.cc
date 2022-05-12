// implementation of the G4MuDecSteppingAction class
#include "G4MuDecSimulator.h"
#include "G4MuDecSteppingAction.h"
#include "G4MuDecConstruction.h"
#include "G4MuDecEventAction.h"
#include "G4MuDecTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"

#include "DetectorSimData.h"
#include "SimData.h"
#include "Detector.h"

G4MuDecSteppingAction::G4MuDecSteppingAction(const G4MuDecConstruction* det, G4MuDecEventAction* G4event, Event& theEvent)
	:	G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
		fEvent(theEvent)
{

}

G4MuDecSteppingAction::~G4MuDecSteppingAction()
{
}

void
G4MuDecSteppingAction::UserSteppingAction(const G4Step* step)
{
		
	DetectorSimData& detSimData = fEvent.GetSimData().GetDetectorSimData();
	auto& muonDecayIDs = detSimData.GetMuonDecayID();

	// 1. Detect if parent muons stop in water
	// 2. Track electrons / positrons in water

	auto track = step->GetTrack();
	const G4ParticleDefinition* particle = track->GetParticleDefinition();

	// reject photons
	if (particle == G4OpticalPhoton::OpticalPhotonDefinition())
		return;

	int trackId = track->GetTrackID();
	int parentId = track->GetParentID();

	// get physical volume name
	//G4String physVolName = track->GetVolume()->GetName();


	if ( (particle->GetParticleName() == "e-" || particle->GetParticleName() == "e+") && parentId == 1) {
		const auto* process = track->GetCreatorProcess();
		if (process->GetProcessType() == fDecay) {
			// std::cout << "[DEBUG] G4MuDecSteppingAction: This " << particle->GetParticleName() << " (" << trackId << ") was produced via " << process->GetProcessName() << " from parent ID " << parentId << std::endl;
			//double electronEnergy = track->GetTotalEnergy();
			//std::cout << "Michel_Electron " << trackId << " Energy " << electronEnergy / CLHEP::MeV << std::endl; 
			muonDecayIDs.insert(trackId);
		
		}
	}
}

