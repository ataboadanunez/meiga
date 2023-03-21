// implementation of the G4TandarSteppingAction class
#include "G4TandarSimulator.h"
#include "G4TandarSteppingAction.h"
#include "G4TandarDetectorConstruction.h"
#include "G4TandarEventAction.h"
#include "G4TandarTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"

#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"

G4TandarSteppingAction::G4TandarSteppingAction(const G4TandarDetectorConstruction* det, G4TandarEventAction* G4event, Event& theEvent)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
    fEvent(theEvent)
{
}

G4TandarSteppingAction::~G4TandarSteppingAction()
{
}

void
G4TandarSteppingAction::UserSteppingAction(const G4Step* step)
{
	
}
