// implementation of the G4LeadSteppingAction class
#include "G4LeadSimulator.h"
#include "G4LeadSteppingAction.h"
#include "G4LeadDetectorConstruction.h"
#include "G4LeadEventAction.h"
#include "G4LeadTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"

#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"

#include <G4OpticalPhoton.hh>

using namespace std;

G4LeadSteppingAction::G4LeadSteppingAction(Event& theEvent)
	: G4UserSteppingAction(),
    	fEvent(theEvent)
{

	if (fEvent.GetSimData().GetSimulationMode() == SimData::SimulationMode::eFast) {
		cout << "[INFO] G4LeadSteppingAction::G4LeadSteppingAction: Running Simulation in Fast mode." << endl;
		cout << "[INFO] G4LeadSteppingAction::G4LeadSteppingAction: Optical photons will be killed! " << endl;
	}

}

G4LeadSteppingAction::~G4LeadSteppingAction()
{

}

void
G4LeadSteppingAction::UserSteppingAction(const G4Step* step)
{

	G4Track *track = step->GetTrack();
	// if fast mode activated, kill optical photons
	if (fEvent.GetSimData().GetSimulationMode() == SimData::SimulationMode::eFast) {
		if (track->GetParticleDefinition() == G4OpticalPhoton::OpticalPhoton())
			track->SetTrackStatus(fStopAndKill);
	}
	
}
