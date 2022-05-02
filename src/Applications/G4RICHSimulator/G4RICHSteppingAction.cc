// implementation of the G4RICHSteppingAction class
#include "G4RICHSimulator.h"
#include "G4RICHSteppingAction.h"
#include "G4RICHConstruction.h"
#include "G4RICHEventAction.h"
#include "G4RICHTrackingAction.h"

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

G4RICHSteppingAction::G4RICHSteppingAction(const G4RICHConstruction* det, G4RICHEventAction* G4event, Event& theEvent)
	:	G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
		fEvent(theEvent)
{

}

G4RICHSteppingAction::~G4RICHSteppingAction()
{
}

void
G4RICHSteppingAction::UserSteppingAction(const G4Step* step)
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
			// std::cout << "[DEBUG] G4RICHSteppingAction: This " << particle->GetParticleName() << " (" << trackId << ") was produced via " << process->GetProcessName() << " from parent ID " << parentId << std::endl;
			//double electronEnergy = track->GetTotalEnergy();
			//std::cout << "Michel_Electron " << trackId << " Energy " << electronEnergy / CLHEP::MeV << std::endl; 
			muonDecayIDs.insert(trackId);
		
		}
	}
}

