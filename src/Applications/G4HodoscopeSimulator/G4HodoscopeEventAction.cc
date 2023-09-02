#include "G4HodoscopeEventAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "OptDeviceSimData.h"

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

	// set total deposited energy at Hodoscope detector
	// as the sum of the deposits on each bar at the event
	// the deposits per bar are calculated by the G4MScintillatorBarAction

	// loop over detectors in the event
	for (auto detIt = fEvent.DetectorRange().begin(); detIt != fEvent.DetectorRange().end(); detIt++) {

		// get current detector data
		Detector& currDet = detIt->second;
		const unsigned int detId = currDet.GetId();
		
		// skip if detector is not an hodoscope
		if (currDet.GetType() != Detector::eHodoscope)
			continue;

		// get detector simdata
		DetectorSimData &detSimData = fEvent.GetSimData().GetDetectorSimData(detId);
		// get total deposited energy at the event
		double totalEdep = detSimData.GetTotalEnergyDeposit();
		// set it to the deposited energy vector
		detSimData.SetEnergyDeposit(totalEdep);
		// clear total energy deposit after the event is terminated to reset counter
		detSimData.ClearTotalEnergyDeposit();


	}

}


