#include "G4MPMTAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "OptDeviceSimData.h"
#include "OptDevice.h"
#include "CorsikaUtilities.h"

#include <G4Step.hh>
#include <G4TouchableHistory.hh>
#include <G4RunManager.hh>
#include <G4OpticalPhoton.hh>

using namespace std;

G4MPMTAction::G4MPMTAction(const G4String& name, const G4int dId, const G4int oId, Event& theEvent) :
		G4VSensitiveDetector(name),
		fEvent(theEvent),
		fDetectorId(dId),
		fOptDeviceId(oId)

	{ 
		
		cout << "[INFO] G4Models::G4MPMTAction: Registering Sensitive Detector " << name << endl;
		SimData& simData = fEvent.GetSimData();
		DetectorSimData& detSimData = simData.GetDetectorSimData(fDetectorId);
		detSimData.MakeOptDeviceSimData(fOptDeviceId);
		// OptDeviceSimData& OptDeviceSimData = detSimData.GetOptDeviceSimData(fOptDeviceId);

	}

void
G4MPMTAction::Initialize(G4HCofThisEvent* const /*hce*/)
{
	

}

void
G4MPMTAction::EndOfEvent(G4HCofThisEvent* const /*hce*/)
{
	

	// gets current particle in the event to stream the number of PE for each particle
	Particle currentParticle = fEvent.GetSimData().GetCurrentParticle();
	int partId = currentParticle.GetParticleId();
	// time distribution for different particles

	Particle::Component particleComponent = currentParticle.GetComponent(/*particleType*/);

	DetectorSimData& detSimData = fEvent.GetSimData().GetDetectorSimData(fDetectorId);
	OptDeviceSimData& odSimData = detSimData.GetOptDeviceSimData(fOptDeviceId);
	int charge = fPETime.size();
	
	// add total PE time distribution
	odSimData.AddPETimeDistribution(fPETime);
	// add components PE time distribution
	odSimData.AddPETimeDistribution(particleComponent, fPETimeComp);
	odSimData.AddPETimeDistribution(Particle::eMuonDecay, fPETimeMuDecay);
	odSimData.AddCharge(charge);

	fPETime.clear();
	fPETimeComp.clear();
	fPETimeMuDecay.clear();

}

G4bool
G4MPMTAction::ProcessHits(G4Step* const step, G4TouchableHistory* const /*rOHist*/)
{
	
	// reject particle in case it is not a photon
	if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
		return true;

	// for RICH paper: get ParentID of photons
	int parentId = step->GetTrack()->GetParentID();

	// get time and energy of photons
	const double time = step->GetPreStepPoint()->GetGlobalTime() / CLHEP::second;
	if (time >= 1*CLHEP::second)
		return true;

	auto& pmt = fEvent.GetDetector(fDetectorId).GetOptDevice(fOptDeviceId);
	double energy = step->GetPreStepPoint()->GetKineticEnergy() / CLHEP::eV;
	// kill if photon energy is out of PMT range
	if (energy < pmt.GetOpticalRange()[0]  || energy > pmt.GetOpticalRange()[1]) 
		return true; 

	SimData& simData = fEvent.GetSimData();
	if (simData.GetSimulationMode() == SimData::SimulationMode::eFull) {
		
		// kill according to PMT quantum efficiency
		if (!pmt.IsPhotonDetected(energy)) 
			return true;
	}
	
	
	DetectorSimData& detSimData = simData.GetDetectorSimData();
	const auto& muDecayIDs = detSimData.GetMuonDecayID();

	if (muDecayIDs.find(parentId) != muDecayIDs.end()) {
		//cout << "[DEBUG] G4Models::G4MPMTAction: Photon from MUON DECAY was detected! " << endl;
		fPETimeMuDecay.push_back(time);
		NumPEMuDecay += 1;
	}

	string procName = step->GetTrack()->GetCreatorProcess()->GetProcessName();
	

	// add photon time to SimData
	fPETime.push_back(time);
	fPETimeComp.push_back(time);
	
	return true;

}
	


