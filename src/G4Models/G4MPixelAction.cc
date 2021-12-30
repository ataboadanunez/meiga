#include "G4MPixelAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "SiPMSimData.h"
#include "Pixel.h"

#include <G4Step.hh>
#include <G4TouchableHistory.hh>
#include <G4RunManager.hh>
#include <G4OpticalPhoton.hh>

G4MPixelAction::G4MPixelAction(const G4String& name, const G4int mId, const G4int sId, Event& theEvent) :
    G4VSensitiveDetector(name),
    fEvent(theEvent),
    fModuleId(mId),
    fPixelId(sId)

  { 
  	
  	std::cout << "[INFO] G4Models::G4MPixelAction: Registering Pixel " << name << std::endl;
  	SimData& simData = fEvent.GetSimData();
  	DetectorSimData& detSimData = simData.GetDetectorSimData(mId);
 		detSimData.MakeSiPMSimData(sId);
 		SiPMSimData& PixelSimData = detSimData.GetSiPMSimData(fPixelId);

 		fPETimeDistribution = PixelSimData.PETimeDistributionRange();

  }

void
G4MPixelAction::Initialize(G4HCofThisEvent* const /*hce*/)
{
	
	fPETime = new std::vector<double>();

}

void
G4MPixelAction::EndOfEvent(G4HCofThisEvent* const /*hce*/)
{
  fEvent.GetSimData().GetDetectorSimData(fModuleId).GetSiPMSimData(fPixelId).AddPETimeDistribution(fPETime);
	//fPETimeDistribution->push_back(fPETime);

}

G4bool
G4MPixelAction::ProcessHits(G4Step* const step, G4TouchableHistory* const /*rOHist*/)
{
	// If it is not an opticalphoton, continue
  if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    return true;

  SimData& simData = fEvent.GetSimData();

  /*
  	Retrieve current physical volumen and its mother (Pixel and module IDs)
		According to DetectorConstructor, Pixel has its mother volume in the Scintillator Bar.
		therefore in order to get the Module ID we need to do GetVolume(3) of the step

		Module Casing (3)  -> Bar Coating (2) -> Scintillator bar (1) -> Pixel (0)
  */

  // for Pixel
  G4VPhysicalVolume* const currentPhysVol = step->GetPreStepPoint()->GetPhysicalVolume();
  // for Module (Casing)
#warning "Avoid hard-coded numbers. Look for better way to retrieve mother volume!"
  G4VPhysicalVolume* const motherPhysVol = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume(3);
  const G4int PixelId = currentPhysVol->GetCopyNo();
  const G4int moduleId = motherPhysVol->GetCopyNo();

  G4double photonEnergy = step->GetTrack()->GetTotalEnergy() / (1*CLHEP::eV);
#warning "Add SiPM / PMT cases"
  Pixel& Pixel = fEvent.GetDetector().GetPixel(PixelId);

	if (Pixel.IsPhotonDetected(photonEnergy)) {
		// get detector sim data by module ID
		std::cout << "[DEBUG] G4Models::G4MPixelAction: Photon arrived to Pixel!! " << PixelId << std::endl;
		DetectorSimData& detSimData = simData.GetDetectorSimData(moduleId);
		//detSimData.MakePixelSimData(PixelId);
		SiPMSimData& PixelSimData = detSimData.GetSiPMSimData(PixelId);
 		//unsigned int PixelIdFwk = PixelSimData.GetId();
 		const double time = step->GetPreStepPoint()->GetGlobalTime() / (1*CLHEP::ns);
#warning "Add SiPM / PMT cases"
		PixelSimData.AddPhotonEnergy(photonEnergy);
		PixelSimData.AddPhotonTime(time);
 		
		fPETime->push_back(time);

  }
 	

	return true;
}

