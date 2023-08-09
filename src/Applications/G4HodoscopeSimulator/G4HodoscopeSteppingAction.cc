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

#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"

#include <G4OpticalPhoton.hh>

using namespace std;

G4HodoscopeSteppingAction::G4HodoscopeSteppingAction(G4HodoscopeEventAction* G4event, Event& theEvent)
	: G4UserSteppingAction(),
		fEventAction(G4event),
    fEvent(theEvent)
{

}

G4HodoscopeSteppingAction::~G4HodoscopeSteppingAction()
{

}

void
G4HodoscopeSteppingAction::UserSteppingAction(const G4Step* step)
{

}
