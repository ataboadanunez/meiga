#include "G4MOptDeviceAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "OptDeviceSimData.h"
#include "OptDevice.h"

#include <G4Step.hh>
#include <G4TouchableHistory.hh>
#include <G4RunManager.hh>
#include <G4OpticalPhoton.hh>

G4MOptDeviceAction::G4MOptDeviceAction(const G4String& name, const G4int dId, const G4int oId, Event& theEvent) :
		G4VSensitiveDetector(name),
		fEvent(theEvent),
		fDetectorId(dId),
		fOptDeviceId(oId)

	{ 
		
		std::cout << "[INFO] G4Models::G4MOptDeviceAction: Registering OptDevice " << name << std::endl;
		SimData& simData = fEvent.GetSimData();
		DetectorSimData& detSimData = simData.GetDetectorSimData(fDetectorId);
		detSimData.MakeOptDeviceSimData(fOptDeviceId);
		OptDeviceSimData& OptDeviceSimData = detSimData.GetOptDeviceSimData(fOptDeviceId);

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
	fEvent.GetSimData().GetDetectorSimData(fDetectorId).GetOptDeviceSimData(fOptDeviceId).AddPETimeDistribution(fPETime);
	//fPETimeDistribution->push_back(fPETime);

}

G4bool
G4MOptDeviceAction::ProcessHits(G4Step* const step, G4TouchableHistory* const /*rOHist*/)
{
	// If it is not an opticalphoton, continue
	if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
		return true;

	SimData& simData = fEvent.GetSimData();

	G4double photonEnergy = step->GetTrack()->GetTotalEnergy() / (1*CLHEP::eV);
	OptDevice& optDevice = fEvent.GetDetector(fDetectorId).GetOptDevice(fOptDeviceId);

	// check if photon is detected according to its energy and quantum efficiency
	if (optDevice.IsPhotonDetected(photonEnergy)) {
		DetectorSimData& detSimData = simData.GetDetectorSimData(fDetectorId);
		OptDeviceSimData& optDeviceSimData = detSimData.GetOptDeviceSimData(fOptDeviceId);
		const double time = step->GetPreStepPoint()->GetGlobalTime() / (1*CLHEP::ns);
		optDeviceSimData.AddPhotonEnergy(photonEnergy);
		optDeviceSimData.AddPhotonTime(time);
		
		fPETime->push_back(time);

	}
	

	return true;
}

