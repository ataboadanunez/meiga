// implementation of the G4ExSteppingAction class
#include "G4ExSimulator.h"
#include "G4ExSteppingAction.h"
#include "G4ExDetectorConstruction.h"
#include "G4ExEventAction.h"
#include "G4ExTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"

#include "SimData.h"
#include "SiPMSimData.h"
#include "Detector.h"
#include "Module.h"

G4ExSteppingAction::G4ExSteppingAction(const G4ExDetectorConstruction* det, G4ExEventAction* G4event, Event& MSevent)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
    fEvent(MSevent)
{
}

G4ExSteppingAction::~G4ExSteppingAction()
{
}

void
G4ExSteppingAction::UserSteppingAction(const G4Step* step)
{
  
  // kill non-primary particles to speed up 
	auto track = step->GetTrack();
	if (track->GetParentID() != 0) {
		G4cout << "[DEBUG] G4ExSteppingAction: Secondary Particle " << track->GetParticleDefinition()->GetParticleName() << " was killed! " << G4endl;
		track->SetTrackStatus(fStopAndKill);
	}
}
