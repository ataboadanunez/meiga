// application headers
#include "G4ExSimulator.h"
#include "G4ExSteppingAction.h"
#include "G4ExDetectorConstruction.h"
#include "G4ExEventAction.h"
#include "G4ExTrackingAction.h"
// Geant4 headers
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4OpticalPhoton.hh"
// Meiga headers
#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"

G4ExSteppingAction::G4ExSteppingAction(const G4ExDetectorConstruction* det, G4ExEventAction* G4event, Event& theEvent)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
    fEvent(theEvent)
{
}

G4ExSteppingAction::~G4ExSteppingAction()
{
}

void
G4ExSteppingAction::UserSteppingAction(const G4Step* step)
{
  
 
}
