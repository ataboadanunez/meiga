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

#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"

G4HodoscopeSteppingAction::G4HodoscopeSteppingAction(const G4HodoscopeDetectorConstruction* det, G4HodoscopeEventAction* G4event, Event& theEvent)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
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
