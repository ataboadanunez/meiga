// implementation of the G4AndesSteppingAction class
#include "G4AndesSimulator.h"
#include "G4AndesSteppingAction.h"
#include "G4AndesDetectorConstruction.h"
#include "G4AndesEventAction.h"
#include "G4AndesTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"

#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"

G4AndesSteppingAction::G4AndesSteppingAction(const G4AndesDetectorConstruction* det, G4AndesEventAction* G4event, Event& theEvent)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
		fEvent(theEvent)
{
}

G4AndesSteppingAction::~G4AndesSteppingAction()
{
}

void
G4AndesSteppingAction::UserSteppingAction(const G4Step* step)
{
	
	//Particle& currParticle = G4AndesSimulator::currentParticle;
	SimData& simData = fEvent.GetSimData();
	double groundThickness = simData.GetGroundThickness();
	double depth = -0.5*groundThickness;

	// kill non-primary particles to speed up 
	G4Track* track = step->GetTrack();
	
	if (track->GetParentID() != 0) {
		track->SetTrackStatus(fStopAndKill);	
	}
	else {
		// here we are left with primary particles (muons)
		double currentEnergy = track->GetTotalEnergy();
		G4ThreeVector trackPosition = track->GetPosition();

		Particle& currParticle = G4AndesSimulator::currentParticle;
		double fKineticEnergy = currParticle.GetKineticEnergy();
		
		double posZ = trackPosition[2];
		// get muon energy at last step in ground
		G4String stepVolumeName = track->GetVolume()->GetName();
		if (stepVolumeName == "Ground" && step->IsLastStepInVolume() && posZ)
			std::cout << "TrackStoppingInfo ParticleID " << currParticle.GetParticleId() << " LastStepEnergy_GeV " << currentEnergy / CLHEP::GeV << " depth_m " << posZ / CLHEP::m << " Initial Energy "  << fKineticEnergy / CLHEP::GeV << std::endl;

	}
}
