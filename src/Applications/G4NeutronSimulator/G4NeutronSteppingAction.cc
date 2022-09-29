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
	Event::Config &cfg = fEvent.GetConfig();
	std::string outFileName = cfg.fOutputFileName;
	// create output file
	fOutputFile = new std::ofstream(outFileName.c_str());
	*fOutputFile << "ParticleID InitialEnergy_GeV LastStepEnergy_GeV depth_m"  << std::endl;
}

G4NeutronSteppingAction::~G4NeutronSteppingAction()
{
	fOutputFile->close();
}

void
G4NeutronSteppingAction::UserSteppingAction(const G4Step* step)
{
  
  //Particle& currParticle = G4AndesSimulator::currentParticle;
	SimData& simData = fEvent.GetSimData();

  // kill non-primary particles to speed up 
	G4Track* track = step->GetTrack();
	
	if (track->GetParentID() != 0) {
		track->SetTrackStatus(fStopAndKill);	
	}
	else {
		// here we are left with primary particles (neutrons)
		double currentEnergy = track->GetTotalEnergy();
		G4ThreeVector trackPosition = track->GetPosition();

		Particle& currParticle = G4NeutronSimulator::currentParticle;
		double fKineticEnergy = currParticle.GetKineticEnergy();
		
		double posZ = trackPosition[2];
		// get muon energy at last step in ground
		G4String stepVolumeName = track->GetVolume()->GetName();
		if (stepVolumeName == "Ground" && step->IsLastStepInVolume() && posZ) {
			// std::cout << "TrackStoppingInfo ParticleID " << currParticle.GetParticleId() << " InitialEnergy_GeV " << fKineticEnergy / CLHEP::GeV << " LastStepEnergy_GeV " << currentEnergy / CLHEP::GeV << " depth_m " << posZ / CLHEP::m << std::endl;
			*fOutputFile << currParticle.GetParticleId() << " " << fKineticEnergy / CLHEP::GeV << " " << currentEnergy / CLHEP::GeV << " " << posZ / CLHEP::m << std::endl;
		}
			
	}
 
}
