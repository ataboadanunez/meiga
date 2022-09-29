#include "G4RockFiberAction.h"
#include "G4RockTrackingAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "SiPMSimData.h"

#include <G4Step.hh>
#include <G4TouchableHistory.hh>
#include "G4UImanager.hh"
#include <G4RunManager.hh>
#include <G4OpticalPhoton.hh>

using namespace std;

G4RockFiberAction::G4RockFiberAction(const G4String& name, Event& theEvent) :
	G4VSensitiveDetector(name),
	fEvent(theEvent)

	{ 
		cout << "[DEBUG] G4RockFiberAction: Registering Fiber " << name << endl;
	}

void
G4RockFiberAction::Initialize(G4HCofThisEvent* const /*hce*/)
{	

	fNumberOfPhotons = 0;
}

void
G4RockFiberAction::EndOfEvent(G4HCofThisEvent* const /*hce*/)
{

	if (fNumberOfPhotons)
		cout << "[DEBUG] G4RockFiberAction::EndOfEvent: Number of Photons registered in Fiber = " << fNumberOfPhotons << endl;
}


G4bool
G4RockFiberAction::ProcessHits(G4Step* const step, G4TouchableHistory* const /*rOHist*/)
{
	// If it is not an opticalphoton, continue
	if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
		return true;

	// fiber volume
  G4VPhysicalVolume* const currentPhysVol = step->GetPreStepPoint()->GetPhysicalVolume();
  const G4int fiberId = currentPhysVol->GetCopyNo();
	// auto touchable = (step->GetPreStepPoint()->GetTouchable()); 
 	// auto fiberNumber = touchable->GetReplicaNumber(1);
 	if (step->IsLastStepInVolume()) {
 		cout << "[DEBUG] G4RockFiberMAction: Photon with track " << step->GetTrack()->GetTrackID() << " detected in fiber = " << fiberId << endl;
 		fNumberOfPhotons+=1;
 		//G4UImanager* fUImanager = G4UImanager::GetUIpointer();
 		//fUImanager->ApplyCommand("/tracking/verbose 1");
 		//step->GetTrack()->SetTrackStatus(fStopAndKill);

 	}
 		

	return true;
}


