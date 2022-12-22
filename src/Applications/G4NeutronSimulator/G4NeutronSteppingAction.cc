// application headers
#include "G4NeutronSimulator.h"
#include "G4NeutronSteppingAction.h"
#include "G4NeutronDetectorConstruction.h"
#include "G4NeutronEventAction.h"
#include "G4NeutronTrackingAction.h"
// Geant4 headers
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4OpticalPhoton.hh"
// Meiga headers
#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"

G4NeutronSteppingAction::G4NeutronSteppingAction(const G4NeutronDetectorConstruction* det, G4NeutronEventAction* G4event, Event& theEvent)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
    fEvent(theEvent)
{
	
	std::string outFileName = "groundTrackingInfo.dat";
	// create output file
	fOutputFile = new std::ofstream(outFileName.c_str());
	*fOutputFile << "ParticleID StepVolume InitialEnergy_GeV CurrentEnergy_GeV depth_m"  << std::endl;
}

G4NeutronSteppingAction::~G4NeutronSteppingAction()
{
	fOutputFile->close();
}

void
G4NeutronSteppingAction::UserSteppingAction(const G4Step* step)
{
  
 
	G4Track* track = step->GetTrack();
	G4String stepVolumeName = track->GetVolume()->GetName();

	// save only primary particles (neutrons)
	if (track->GetParentID() == 0) {
		double fTrackEnergy = track->GetTotalEnergy();
		G4ThreeVector trackPosition = track->GetPosition();

		Particle& currParticle = G4NeutronSimulator::currentParticle;
		double fInitialEnergy = currParticle.GetTotalEnergy();
		
		double posZ = trackPosition[2];
		// get muon energy at last step in ground
		// std::cout << "[DEBUG] G4NeutronDetectorConstruction::G4UserSteppingAction: stepVolumeName " << stepVolumeName << " posZ " << posZ << std::endl;  
		if (step->IsLastStepInVolume()) {
			// std::cout << "TrackStoppingInfo ParticleID " << currParticle.GetParticleId() << " InitialEnergy_GeV " << fKineticEnergy / CLHEP::GeV << " LastStepEnergy_GeV " << currentEnergy / CLHEP::GeV << " depth_m " << posZ / CLHEP::m << std::endl;
			*fOutputFile << currParticle.GetParticleId() << " " << stepVolumeName << " " << fInitialEnergy / CLHEP::GeV << " " << fTrackEnergy / CLHEP::GeV << " " << posZ / CLHEP::m << std::endl;
		}
	}
	
			
}
 
