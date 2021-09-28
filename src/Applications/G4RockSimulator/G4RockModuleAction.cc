#include "G4RockModuleAction.h"
#include "G4RockTrackingAction.h"
#include "SimData.h"
#include "DetectorSimData.h"

#include <G4Step.hh>
#include <G4TouchableHistory.hh>
#include <G4RunManager.hh>
#include <G4OpticalPhoton.hh>

G4RockModuleAction::G4RockModuleAction(const G4String& name, const G4int id, Event& theEvent) :
    G4VSensitiveDetector(name),
    fEvent(theEvent)
  { 
  	
  	std::cout << "[DEBUG] G4RockModuleAction: Registering Detector Module " << name << std::endl;
  	// create detector sim data
  	SimData& simData = fEvent.GetSimData();
  	simData.MakeDetectorSimData(id);

  }

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
  //simData.MakeDetectorSimData(moduleId);
  DetectorSimData& detSimData = simData.GetDetectorSimData(moduleId);
  //const int moduleIdFwk = detSimData.GetModuleId();

  // Add muon / particle counter

	return true;
}