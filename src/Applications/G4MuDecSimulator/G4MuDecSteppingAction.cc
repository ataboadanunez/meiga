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

G4MuDecSteppingAction::G4MuDecSteppingAction(const G4MuDecConstruction* det, G4MuDecEventAction* G4event, Event& theEvent, bool countCerenkov)
	:	G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
		fEvent(theEvent),
		fCountCerenkov(countCerenkov)
{

}

G4MuDecSteppingAction::~G4MuDecSteppingAction()
{
}

void
G4MuDecSteppingAction::UserSteppingAction(const G4Step* step)
{
		
	// DetectorSimData& detSimData = fEvent.GetSimData().GetDetectorSimData();
	// auto& muonDecayIDs = detSimData.GetMuonDecayID();

	// 1. Detect if parent muons stop in water
	// 2. Track electrons / positrons in water

	auto track = step->GetTrack();
	int parentId = track->GetParentID();
	int trackId = track->GetTrackID();

	const G4ParticleDefinition* particle = track->GetParticleDefinition();

	// reject photons
	if (particle == G4OpticalPhoton::OpticalPhotonDefinition())
		return;

	// get energy deposit of primaries in water
	if ((trackId == 1) && (track->GetVolume()->GetName() == "physTank")) {
		double depE = step->GetTotalEnergyDeposit();
		fEventAction->AddEnergyDeposit(depE);
	// get step-length of muons inside water volume
	if (fCountCerenkov) {
			double stepLength = step->GetStepLength();
			fEventAction->AddStepLength(stepLength);
		}	
	}
	
}

