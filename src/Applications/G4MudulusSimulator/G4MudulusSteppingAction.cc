#include "G4MudulusSimulator.h"
#include "G4MudulusSteppingAction.h"
#include "G4MudulusDetectorConstruction.h"
#include "G4MudulusEventAction.h"
#include "G4MudulusTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"

#include "SimData.h"
#include "Detector.h"

G4MudulusSteppingAction::G4MudulusSteppingAction(const G4MudulusDetectorConstruction* det, G4MudulusEventAction* G4event, Event& MSevent)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
    fEvent(MSevent)
{
}

G4MudulusSteppingAction::~G4MudulusSteppingAction()
{
}

void
G4MudulusSteppingAction::UserSteppingAction(const G4Step* step)
{
  
 //  // kill non-primary particles to speed up 
	// auto track = step->GetTrack();
	// if (track->GetParentID() != 0) {
	// 	G4cout << "[DEBUG] G4MudulusSteppingAction: Secondary Particle " << track->GetParticleDefinition()->GetParticleName() << " was killed! " << G4endl;
	// 	track->SetTrackStatus(fStopAndKill);
	// }
}
