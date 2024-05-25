// implementation of the G4MuDecSteppingAction class
#include "G4MuDecSimulator.h"
#include "G4MuDecSteppingAction.h"
#include "G4MuDecConstruction.h"
#include "G4MuDecEventAction.h"
#include "G4MuDecTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"

#include "DetectorSimData.h"
#include "SimData.h"
#include "Detector.h"

G4MuDecSteppingAction::G4MuDecSteppingAction()
	:	G4UserSteppingAction()
{
}

G4MuDecSteppingAction::~G4MuDecSteppingAction()
{
}

void
G4MuDecSteppingAction::UserSteppingAction(const G4Step* step)
{
	
}

