#include "G4MOptDeviceAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "SiPMSimData.h"
#include "OptDevice.h"

#include <G4Step.hh>
#include <G4TouchableHistory.hh>
#include <G4RunManager.hh>
#include <G4OpticalPhoton.hh>

G4MOptDeviceAction::G4MOptDeviceAction(const G4String& name, const G4int mId, const G4int sId, Event& theEvent) :
    G4VSensitiveDetector(name),
    fEvent(theEvent),
    fModuleId(mId),
    fOptDeviceId(sId)

  { 
  	
  	std::cout << "[INFO] G4Models::G4MOptDeviceAction: Registering OptDevice " << name << std::endl;
  	SimData& simData = fEvent.GetSimData();
  	DetectorSimData& detSimData = simData.GetDetectorSimData(mId);
 		detSimData.MakeSiPMSimData(sId);
 		SiPMSimData& OptDeviceSimData = detSimData.GetSiPMSimData(fOptDeviceId);

 		fPETimeDistribution = OptDeviceSimData.PETimeDistributionRange();

  }

void
G4MOptDeviceAction::Initialize(G4HCofThisEvent* const /*hce*/)
{
	
	fPETime = new std::vector<double>();

}

void
G4MOptDeviceAction::EndOfEvent(G4HCofThisEvent* const /*hce*/)
{
  fEvent.GetSimData().GetDetectorSimData(fModuleId).GetSiPMSimData(fOptDeviceId).AddPETimeDistribution(fPETime);
	//fPETimeDistribution->push_back(fPETime);

}

G4bool
G4MOptDeviceAction::ProcessHits(G4Step* const step, G4TouchableHistory* const /*rOHist*/)
{
	// If it is not an opticalphoton, continue
  if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    return true;

  SimData& simData = fEvent.GetSimData();

  /*
  	Retrieve current physical volumen and its mother (OptDevice and module IDs)
		According to DetectorConstructor, OptDevice has its mother volume in the Scintillator Bar.
		therefore in order to get the Module ID we need to do GetVolume(3) of the step

		Module Casing (3)  -> Bar Coating (2) -> Scintillator bar (1) -> OptDevice (0)
  */

  // for OptDevice
  G4VPhysicalVolume* const currentPhysVol = step->GetPreStepPoint()->GetPhysicalVolume();
  // for Module (Casing)
#warning "Avoid hard-coded numbers. Look for better way to retrieve mother volume!"
  G4VPhysicalVolume* const motherPhysVol = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume(3);
  const G4int OptDeviceId = currentPhysVol->GetCopyNo();
  const G4int moduleId = motherPhysVol->GetCopyNo();

  G4double photonEnergy = step->GetTrack()->GetTotalEnergy() / (1*CLHEP::eV);
#warning "Add SiPM / PMT cases"
  OptDevice& OptDevice = fEvent.GetDetector().GetOptDevice(OptDeviceId);

	if (OptDevice.IsPhotonDetected(photonEnergy)) {
		// get detector sim data by module ID
		std::cout << "[DEBUG] G4Models::G4MOptDeviceAction: Photon arrived to " << OptDevice.GetType() << " " << OptDeviceId << std::endl;
		DetectorSimData& detSimData = simData.GetDetectorSimData(moduleId);
		//detSimData.MakeOptDeviceSimData(OptDeviceId);
		SiPMSimData& OptDeviceSimData = detSimData.GetSiPMSimData(OptDeviceId);
 		//unsigned int OptDeviceIdFwk = OptDeviceSimData.GetId();
 		const double time = step->GetPreStepPoint()->GetGlobalTime() / (1*CLHEP::ns);
#warning "Add SiPM / PMT cases"
		OptDeviceSimData.AddPhotonEnergy(photonEnergy);
		OptDeviceSimData.AddPhotonTime(time);
 		
		fPETime->push_back(time);

  }
 	

	return true;
}

