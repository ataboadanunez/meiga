#include "G4RockModuleAction.h"
#include "G4RockTrackingAction.h"
#include "SimData.h"
#include "DetectorSimData.h"

#include <G4Step.hh>
#include <G4TouchableHistory.hh>
#include <G4RunManager.hh>
#include <G4OpticalPhoton.hh>

G4RockModuleAction::G4RockModuleAction(const G4String& name, Event& theEvent) :
    G4VSensitiveDetector(name),
    fEvent(theEvent)
  { }

G4bool
G4RockModuleAction::ProcessHits(G4Step* const step, G4TouchableHistory* const /*rOHist*/)
{
	// If it is an opticalphoton, continue
  if (step->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    return true;

  // Retrieve current physical volumen and its name (i.e. the current physical strip)
  G4VPhysicalVolume* const currentPhysVol = step->GetPreStepPoint()->GetPhysicalVolume();
  const G4int moduleId = currentPhysVol->GetCopyNo();

  const int particleId = step->GetTrack()->GetDefinition()->GetPDGEncoding();
  const int trackID = step->GetTrack()->GetTrackID();

  SimData& simData = fEvent.GetSimData();
  simData.MakeDetectorSimData(moduleId);

  DetectorSimData& detSimData = simData.GetDetectorSimData(moduleId);
  const int moduleIdFwk = detSimData.GetModuleId();

  if (step->GetPreStepPoint()->GetStepStatus() == fGeomBoundary) {
  	G4cout << "[DEBUG] G4RockModuleAction: Particle " << particleId << " with track ID " << trackID << " detected in volume " << currentPhysVol->GetName() << " " << moduleId << " " << moduleIdFwk << G4endl;

  	//G4RockTrackingAction* const trackingA = (G4RockTrackingAction*)G4RunManager::GetRunManager()->GetUserTrackingAction();
  	//fG4StationSimulator.AddInjectedParticle(moduleId, stripId, injectedParticle);
  	// Allows to keep memory of the first time the current particle enters in the current strip
  	//trackingA->SetFirstStepInVolume(false);

	}

	return true;
}