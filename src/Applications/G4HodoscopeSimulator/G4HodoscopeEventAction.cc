#include "G4HodoscopeEventAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "OptDeviceSimData.h"
#include "Scintillator.h"

using namespace std;

G4HodoscopeEventAction::G4HodoscopeEventAction(Event& theEvent) : 
	fEvent(theEvent),
	G4UserEventAction()
{
}

G4HodoscopeEventAction::~G4HodoscopeEventAction()
{
}

void
G4HodoscopeEventAction::BeginOfEventAction(const G4Event*)
{
}

void 
G4HodoscopeEventAction::EndOfEventAction(const G4Event*)
{
	long int mEventCounter = fEvent.GetSimData().EventCounter();
	cout << "[INFO] Summary of Event " << mEventCounter << ": " << endl;
	// loop over detectors in the event
	for (auto &pair : fEvent.DetectorRange()) {

		// get current detector data
		Detector& currDet = *(pair.second);
		const unsigned int detId = currDet.GetId();
		
		// skip if detector is not an hodoscope
		if (currDet.GetType() != Detector::eHodoscope)
			continue;

		// get detector simdata
		DetectorSimData &detSimData = fEvent.GetSimData().GetDetectorSimData(detId);
		// get total deposited energy at the event
		double totalEdep = detSimData.GetTotalEnergyDeposit();
		int nBars;
		const Scintillator *scintDet = dynamic_cast<const Scintillator*>(&currDet);
		if(scintDet) {
			nBars = scintDet->GetNBars();
		}

		// initialize string to 0s. total number of bars x2 since is a grid of 2 x NBars
		string binaryString(2*nBars, '0');
		
		// loop over optical devices (only in Full mode)
		if (fEvent.GetSimData().GetSimulationMode() == SimData::SimulationMode::eFull) {
			
			for (auto odIt = currDet.OptDeviceRange().begin(); odIt != currDet.OptDeviceRange().end(); odIt++) {

				auto& currOd = odIt->second;
				int odId = currOd.GetId();
				// access to optical device signals
				OptDeviceSimData& odSimData = detSimData.GetOptDeviceSimData(odId);
				if (!odSimData.HasPETimeDistribution())
					continue;

				const auto peTimeDistributionRange = odSimData.GetPETimeDistribution();
				
				// access to PETimeDistribution element by run ID (should match the vector element)
				auto peTime = peTimeDistributionRange.at(mEventCounter);
				odSimData.AddCharge(peTime.size());
			}
		}

		for (int hitBarIndex : detSimData.GetHitBarIndices()) {
	        binaryString[hitBarIndex-1] = '1'; // Set the bit at hitBarIndex to 1
	  	}
	  	detSimData.AddBinaryCounter(binaryString);
	  	detSimData.ClearHitBarIndices();
		
		detSimData.SetEnergyDeposit(totalEdep);
		detSimData.ClearTotalEnergyDeposit();
		cout << "[INFO] Detector " << currDet.GetId() << ": Total Energy Deposit = " << totalEdep / CLHEP::MeV 
													  << " MeV, Binary Counter = " << binaryString << endl;

	}

}


