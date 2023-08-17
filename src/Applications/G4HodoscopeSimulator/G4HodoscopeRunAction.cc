// implementation of the G4HodoscopeRunAction class
#include "G4Timer.hh"
#include "G4Run.hh"
#include "g4root.hh"
#include "G4AccumulableManager.hh"
#include "G4HodoscopeRunAction.h"

#include <bitset>

//#include "histosRun.hh"

//#include <TFile.h>

using namespace std;

G4HodoscopeRunAction::G4HodoscopeRunAction(Event& theEvent)
 : G4UserRunAction(),
 fEvent(theEvent)
{
	G4cout << "...G4HodoscopeRunAction..." << G4endl;
	
}


G4HodoscopeRunAction::~G4HodoscopeRunAction()
{	

}


void 
G4HodoscopeRunAction::BeginOfRunAction(const G4Run* aRun)
{
	
}


void 
G4HodoscopeRunAction::EndOfRunAction(const G4Run* aRun)
{ 
	

	// loop over scintillator panels (independent detectors) 	

	for (auto detIt = fEvent.DetectorRange().begin(); detIt != fEvent.DetectorRange().end(); detIt++) {

		G4int g4RunId = aRun->GetRunID();

		Detector& currDet = detIt->second;
		const unsigned int detId = currDet.GetId();

		DetectorSimData& detSimData = fEvent.GetSimData().GetDetectorSimData(detId);

		int nBars = currDet.GetNBars();
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
				auto peTime = peTimeDistributionRange.at(g4RunId);
				odSimData.AddCharge(peTime.size());

			}

		}

		for (int hitBarIndex : detSimData.GetHitBarIndices()) {
	        binaryString[hitBarIndex-1] = '1'; // Set the bit at hitBarIndex to 1
	  }

	  detSimData.AddBinaryCounter(binaryString);
	  // this is necessary to not overwrite the binary counter!
	  detSimData.ClearHitBarIndices();

	}

}
