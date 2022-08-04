#include "G4MDetectorAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "CorsikaUtilities.h"

#include <G4Step.hh>
#include <G4Track.hh>
#include <G4TouchableHistory.hh>


using namespace std;

G4MDetectorAction::G4MDetectorAction(const G4String& name, const G4int dId, Event& theEvent) : 
	G4VSensitiveDetector(name),
	fEvent(theEvent),
	fDetectorId(dId)

	{

		cout << "[INFO] G4Models::G4MDetectorAction: Registering Sensitive Detector " << name << endl;

	}


void 
G4MDetectorAction::Initialize(G4HCofThisEvent* const /*hce*/)
{
	fEdep = 0;
}

void
G4MDetectorAction::EndOfEvent(G4HCofThisEvent* const /*hce*/)
{

	//cout << "[DEBUG] G4Models::G4MDetectorAction: End of event" << endl;
	DetectorSimData& detSimData = fEvent.GetSimData().GetDetectorSimData(fDetectorId);

	fEdep /= CLHEP::MeV;
	cout << "[DEBUG] G4Models::G4MDetectorAction: Energy deposit = " << fEdep << endl;
	detSimData.SetEnergyDeposit(fEdep);

}

G4bool
G4MDetectorAction::ProcessHits(G4Step* const step, G4TouchableHistory* const /*rOHist*/)
{
	
	//cout << "[DEBUG] G4Models::G4MDetectorAction: Processing hits" << endl;
	auto track = step->GetTrack();
	auto trackId = track->GetTrackID();
	//cout << "[DEBUG] G4Models::G4MDetectorAction: of track " << trackId << endl;
	// get energy deposit of primaries
	if (trackId == 1) {

		fEdep += step->GetTotalEnergyDeposit();
	}

	return true;
}