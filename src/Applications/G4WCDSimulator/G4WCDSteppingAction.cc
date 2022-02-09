// implementation of the G4WCDSteppingAction class
#include "G4WCDSimulator.h"
#include "G4WCDSteppingAction.h"
#include "G4WCDConstruction.h"
#include "G4WCDEventAction.h"
#include "G4WCDTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"

#include "SimData.h"
#include "SiPMSimData.h"
#include "Detector.h"
#include "Module.h"

G4WCDSteppingAction::G4WCDSteppingAction(const G4WCDConstruction* det, G4WCDEventAction* G4event, Event& MSevent)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
    fEvent(MSevent)
{
}

G4WCDSteppingAction::~G4WCDSteppingAction()
{
}

void
G4WCDSteppingAction::UserSteppingAction(const G4Step* step)
{
  
 //  // kill non-primary particles to speed up 
	// auto track = step->GetTrack();
	// if (track->GetParentID() != 0) {
	// 	G4cout << "[DEBUG] G4WCDSteppingAction: Secondary Particle " << track->GetParticleDefinition()->GetParticleName() << " was killed! " << G4endl;
	// 	track->SetTrackStatus(fStopAndKill);
	// }
}
