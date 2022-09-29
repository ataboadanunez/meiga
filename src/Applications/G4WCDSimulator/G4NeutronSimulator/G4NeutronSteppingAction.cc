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
}

G4NeutronSteppingAction::~G4NeutronSteppingAction()
{
}

void
G4NeutronSteppingAction::UserSteppingAction(const G4Step* step)
{
  
 
}
