// implementation of the G4HodoscopeSteppingAction class
#include "G4HodoscopeSimulator.h"
#include "G4HodoscopeSteppingAction.h"
#include "G4HodoscopeDetectorConstruction.h"
#include "G4HodoscopeEventAction.h"
#include "G4HodoscopeTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"

#include "Logger.h"
#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"

#include <G4OpticalPhoton.hh>

using namespace std;

G4HodoscopeSteppingAction::G4HodoscopeSteppingAction(Event &aEvent)
	: G4UserSteppingAction(), fEvent(aEvent)
{

	if (fEvent.GetSimData().GetSimulationMode() == SimData::SimulationMode::eFast) {
		ostringstream msg;
		msg << "Running Simulation in Fast mode: Optical photons will be killed!";
		Logger::Print(msg, INFO, "G4HodoscopeSteppingAction");
	}

}

G4HodoscopeSteppingAction::~G4HodoscopeSteppingAction()
{
}

void
G4HodoscopeSteppingAction::UserSteppingAction(const G4Step* step)
{

	G4Track *track = step->GetTrack();
	// if fast mode activated, kill optical photons
	if (fEvent.GetSimData().GetSimulationMode() == SimData::SimulationMode::eFast) {
		if (track->GetParticleDefinition() == G4OpticalPhoton::OpticalPhoton())
			track->SetTrackStatus(fStopAndKill);
	}

}
