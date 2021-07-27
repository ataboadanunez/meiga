#include "G4RockSiPMAction.h"
#include "G4RockTrackingAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "SiPMSimData.h"

#include <G4Step.hh>
#include <G4TouchableHistory.hh>
#include <G4RunManager.hh>
#include <G4OpticalPhoton.hh>

G4RockSiPMAction::G4RockSiPMAction(const G4String& name, Event& theEvent) :
    G4VSensitiveDetector(name),
    fEvent(theEvent)
  { }

G4bool
G4RockSiPMAction::ProcessHits(G4Step* const step, G4TouchableHistory* const /*rOHist*/)
{
	// If it is not an opticalphoton, continue
  if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    return true;

  SimData& simData = fEvent.GetSimData();

  /*
  	Retrieve current physical volumen and its mother (sipm and module IDs)
		According to DetectorConstructor, SiPM has its mother volume in the Scintillator Bar.
		therefore in order to get the Module ID we need to do GetVolume(3) of the step

		Module Enclosure (3)  -> Bar Coating (2) -> Scintillator bar (1) -> SiPM (0)
  */

  // for SiPM
  G4VPhysicalVolume* const currentPhysVol = step->GetPreStepPoint()->GetPhysicalVolume();
  // for Module (enclosure)
  G4VPhysicalVolume* const motherPhysVol = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume(3);
  const G4int sipmId = currentPhysVol->GetCopyNo();
  const G4int moduleId = motherPhysVol->GetCopyNo();
 	
 	// get detector sim data by module ID
 	DetectorSimData& detSimData = simData.GetDetectorSimData(moduleId);
 	detSimData.MakeSiPMSimData(sipmId);
 	SiPMSimData& sipmSimData = detSimData.GetSiPMSimData(sipmId);
 	unsigned int sipmIdFwk = sipmSimData.GetId();

 	const double time = step->GetPreStepPoint()->GetGlobalTime() / (1*ns);
 	sipmSimData.AddPhotonTime(time);
 	
  G4cout << "[DEBUG] G4RockSiPMAction: Optical Photon detected in volume " << currentPhysVol->GetName() << " " << sipmId << " " << sipmIdFwk << " at Module " << moduleId << G4endl;

	return true;
}