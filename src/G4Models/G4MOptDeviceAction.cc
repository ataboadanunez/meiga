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
		
		SimData& simData = fEvent.GetSimData();
		DetectorSimData& detSimData = simData.GetDetectorSimData(fDetectorId);
		detSimData.MakeOptDeviceSimData(fOptDeviceId);
		// OptDeviceSimData& OptDeviceSimData = detSimData.GetOptDeviceSimData(fOptDeviceId);
		OptDevice& optDevice = fEvent.GetDetector(fDetectorId).GetOptDevice(fOptDeviceId);
		// fPETimeDistribution = OptDeviceSimData.PETimeDistributionRange();

		std::cout << "[INFO] G4Models::G4MOptDeviceAction: Registering OptDevice " << optDevice.GetName() << " " << name << " " << fOptDeviceId << std::endl;

	}

void
G4MOptDeviceAction::Initialize(G4HCofThisEvent* const /*hce*/)
{
	
	// fPETime = new std::vector<double>();

}

void
G4MOptDeviceAction::EndOfEvent(G4HCofThisEvent* const /*hce*/)
{
	
	OptDeviceSimData& odSimData = fEvent.GetSimData().GetDetectorSimData(fDetectorId).GetOptDeviceSimData(fOptDeviceId);
	odSimData.AddPETimeDistribution(fPETime);

	// clear PE time vector
	fPETime.clear();

}

G4VSensitiveDetector *G4MOptDeviceAction::Clone() const
{
    return nullptr;
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
		OptDeviceSimData& odSimData = detSimData.GetOptDeviceSimData(fOptDeviceId);
		// get photon time
		const double time = step->GetPreStepPoint()->GetGlobalTime() / (1*CLHEP::ns);
		odSimData.AddPhotonEnergy(photonEnergy);
		odSimData.AddPhotonTime(time);
		
		fPETime.push_back(time);

	}
	

	return true;
}

