// implementation of the G4CasposoSteppingAction class
#include "G4CasposoSimulator.h"
#include "G4CasposoSteppingAction.h"
#include "G4CasposoDetectorConstruction.h"
#include "G4CasposoEventAction.h"
#include "G4CasposoTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"

#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"

G4CasposoSteppingAction::G4CasposoSteppingAction(const G4CasposoDetectorConstruction* det, G4CasposoEventAction* G4event, Event& theEvent)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
    fEvent(theEvent)
{
}

G4CasposoSteppingAction::~G4CasposoSteppingAction()
{
}

void
G4CasposoSteppingAction::UserSteppingAction(const G4Step* step)
{
  
	//Particle& currParticle = G4CasposoSimulator::currentParticle;
	SimData& simData = fEvent.GetSimData();
	double groundThickness = simData.GetGroundThickness();
	double depth = -0.5*groundThickness;
  // kill non-primary particles to speed up 
	G4Track* track = step->GetTrack();
	if (track->GetParentID() != 0) {
		//track->SetTrackStatus(fStopAndKill);	
	}
	else {
		// here we are left with primary particles (muons)
		double currentEnergy = track->GetTotalEnergy();
		G4ThreeVector trackPosition = track->GetPosition();
		
		double posZ = trackPosition[2];
		// get muon energy at last step in ground
		G4String stepVolumeName = track->GetVolume()->GetName();
		if (stepVolumeName == "Ground" && step->IsLastStepInVolume())
			std::cout << "LastStepEnergy_GeV " << currentEnergy / CLHEP::GeV << " depth_m " << posZ / CLHEP::m << std::endl;

	}
}
