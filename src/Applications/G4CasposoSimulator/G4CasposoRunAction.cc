// implementation of the G4CasposoRunAction class

#include "G4Timer.hh"
#include "G4Run.hh"
#include "g4root.hh"
#include "G4AccumulableManager.hh"
#include "G4CasposoRunAction.h"
//#include "histosRun.hh"

//#include <TFile.h>

using namespace std;

G4CasposoRunAction::G4CasposoRunAction(Event& theEvent)
 : G4UserRunAction(),
 fEvent(theEvent)
{
	G4cout << "...G4CasposoRunAction..." << G4endl;
	string fileName = theEvent.GetSimData().GetOutputFileName();
	fOutFile = std::ofstream(fileName, ofstream::out | ofstream::app);
	G4cout << "[DEBUG] G4CasposoRunAction: output file name = " << fileName << G4endl;
}


G4CasposoRunAction::~G4CasposoRunAction()
{
	fOutFile.close();
}


void 
G4CasposoRunAction::BeginOfRunAction(const G4Run* aRun)
{

	// write header only in run = 0.

	G4int g4RunId = aRun->GetRunID();
	if (g4RunId == 0) {
		Detector& currDet = fEvent.GetDetector(0);
		// loop over OptDevices and print output header
		// #p_momentum Channel_1 Channel_2 ... Channel_n
		fOutFile << "# p_momentum ";
		for (auto odIt = currDet.OptDeviceRange().begin(); odIt != currDet.OptDeviceRange().end(); odIt++) {
			auto& currOd = odIt->second;
			int odId = currOd.GetId();
			//cout << "[DEBUG] Optical Device ID = " << odId << endl;
			fOutFile << odId << " ";
		}

		fOutFile << endl;
	
	}
	
}


void 
G4CasposoRunAction::EndOfRunAction(const G4Run* aRun)
{ 

	// get current particle information
	double particleMomentum = G4CasposoSimulator::currentParticle.GetMomentum();
	fOutFile << particleMomentum / CLHEP::MeV << " ";

	// loop PMT channels and get signal for each injected particle

	G4int g4RunId = aRun->GetRunID();
	Detector& currDet = fEvent.GetDetector(0);

	int detId = currDet.GetId();

	DetectorSimData& detSimData = fEvent.GetSimData().GetDetectorSimData(detId);

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
		
		fOutFile << peTime.size() << " ";
		
	}

	fOutFile << endl;
}
