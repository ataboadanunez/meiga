#include "G4MScintillatorBarAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "CorsikaUtilities.h"

#include <G4Step.hh>
#include <G4Track.hh>
#include <G4TouchableHistory.hh>


using namespace std;

G4MScintillatorBarAction::G4MScintillatorBarAction(const G4String& barName, const G4int detId, const G4int barId, Event& theEvent) : 
	G4VSensitiveDetector(barName),
	fEvent(theEvent),
	fDetectorId(detId),
	fBarId(barId)

	{

		/*
				Implementation of the G4MScintillatorBarAction

				This class processes particle hits in scintillator
				bars. Scintillator bars are registered as independent
				Sensitive Detectors given by their name and ID set at the detector construction.

				detId corresponds to the detector ID from which a bar
				with id barId is part of.

		*/

		cout << "[INFO] G4Models::G4MScintillatorBarAction: Registering Sensitive Detector " << barName << endl;

	}


void 
G4MScintillatorBarAction::Initialize(G4HCofThisEvent* const /*hce*/)
{
	fBarEdep = 0;
}

void
G4MScintillatorBarAction::EndOfEvent(G4HCofThisEvent* const /*hce*/)
{

	DetectorSimData& detSimData = fEvent.GetSimData().GetDetectorSimData(fDetectorId);
	// adding energy deposited at barId in this event
	detSimData.AddEnergyDeposit(fBarEdep);

}

G4bool
G4MScintillatorBarAction::ProcessHits(G4Step* const step, G4TouchableHistory* const /*rOHist*/)
{
	

	DetectorSimData& detSimData = fEvent.GetSimData().GetDetectorSimData(fDetectorId);
	
	// process hits of primary particles only
	auto track = step->GetTrack();
	auto trackId = track->GetTrackID();

	if (trackId == 1) {
		detSimData.AddHitBarIndex(fBarId);
		fBarEdep += step->GetTotalEnergyDeposit();
	}

	return true;
}