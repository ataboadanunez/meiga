#include "G4RockSiPMAction.h"
#include "G4RockTrackingAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "SiPMSimData.h"
#include "SiPM.h"

#include <G4Step.hh>
#include <G4TouchableHistory.hh>
#include <G4RunManager.hh>
#include <G4OpticalPhoton.hh>

G4RockSiPMAction::G4RockSiPMAction(const G4String& name, const G4int mId, const G4int sId, Event& theEvent) :
    G4VSensitiveDetector(name),
    fEvent(theEvent),
    fModuleId(mId),
    fSiPMId(sId)

  { 
  	
  	std::cout << "[DEBUG] G4RockSiPMAction: Registering SiPM " << name << std::endl;
  	SimData& simData = fEvent.GetSimData();
  	DetectorSimData& detSimData = simData.GetDetectorSimData(mId);
 		detSimData.MakeSiPMSimData(sId);
 		SiPMSimData& sipmSimData = detSimData.GetSiPMSimData(fSiPMId);

 		fPETimeDistribution = sipmSimData.PETimeDistributionRange();

  }

void
G4RockSiPMAction::Initialize(G4HCofThisEvent* const /*hce*/)
{
	
	fPETime = new std::vector<double>();

}

void
G4RockSiPMAction::EndOfEvent(G4HCofThisEvent* const /*hce*/)
{
  fEvent.GetSimData().GetDetectorSimData(fModuleId).GetSiPMSimData(fSiPMId).AddPETimeDistribution(fPETime);
	//fPETimeDistribution->push_back(fPETime);

}

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

		Module Casing (3)  -> Bar Coating (2) -> Scintillator bar (1) -> SiPM (0)
  */

  // for SiPM
  G4VPhysicalVolume* const currentPhysVol = step->GetPreStepPoint()->GetPhysicalVolume();
  // for Module (Casing)
  G4VPhysicalVolume* const motherPhysVol = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume(3);
  const G4int sipmId = currentPhysVol->GetCopyNo();
  const G4int moduleId = motherPhysVol->GetCopyNo();

  G4double photonEnergy = step->GetTrack()->GetTotalEnergy() / (1*eV);

  SiPM& sipm = fEvent.GetDetector().GetModule().GetSiPM();

  if (sipm.IsPhotonDetected(photonEnergy)) {
  	//std::cout << "[DEBUG] G4RockSiPMAction: Photon with track " << step->GetTrack()->GetTrackID() << " detected in SiPM " << sipmId << std::endl;
 		// get detector sim data by module ID
 		DetectorSimData& detSimData = simData.GetDetectorSimData(moduleId);
 		//detSimData.MakeSiPMSimData(sipmId);
 		SiPMSimData& sipmSimData = detSimData.GetSiPMSimData(sipmId);
 		
 		//unsigned int sipmIdFwk = sipmSimData.GetId();

 		const double time = step->GetPreStepPoint()->GetGlobalTime() / (1*ns);
 		
 		// en offline usan fG4RockSimulator.AddPhotonTime(time) que llama a sipmSimData para cada evento en particular y anade "time" a una distribucion temporal
 		sipmSimData.AddPhotonEnergy(photonEnergy);
 		sipmSimData.AddPhotonTime(time);
 		
 		fPETime->push_back(time);

  }
 	

	return true;
}

