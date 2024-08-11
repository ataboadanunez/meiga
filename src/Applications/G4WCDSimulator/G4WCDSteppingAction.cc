// implementation of the G4WCDSteppingAction class
#include "G4WCDSimulator.h"
#include "G4WCDSteppingAction.h"
#include "G4WCDConstruction.h"
#include "G4WCDEventAction.h"
#include "G4WCDTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"

#include "SimData.h"
#include "Detector.h"

G4WCDSteppingAction::G4WCDSteppingAction()
	: G4UserSteppingAction()
{
}

G4WCDSteppingAction::~G4WCDSteppingAction()
{
}

void
G4WCDSteppingAction::UserSteppingAction(const G4Step* step)
{
}
