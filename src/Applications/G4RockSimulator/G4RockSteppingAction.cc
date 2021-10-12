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

#include "SimData.h"
#include "SiPMSimData.h"
#include "Detector.h"
#include "Module.h"

G4RockSteppingAction::G4RockSteppingAction(const G4RockDetectorConstruction* det, G4RockEventAction* G4event, Event& MSevent)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
    fEvent(MSevent)
{
}

G4RockSteppingAction::~G4RockSteppingAction()
{
}

void
G4RockSteppingAction::UserSteppingAction(const G4Step* step)
{
  
  // kill non-primary particles to speed up 
	auto track = step->GetTrack();
	if (track->GetParentID() != 0) {
		G4cout << "[DEBUG] G4RockSteppingAction: Secondary Particle " << track->GetParticleDefinition()->GetParticleName() << " was killed! " << G4endl;
		track->SetTrackStatus(fStopAndKill);
	}
}
