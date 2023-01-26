#include "G4HodoscopeEventAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "OptDeviceSimData.h"

G4HodoscopeEventAction::G4HodoscopeEventAction(Event& theEvent) : 
	fEvent(theEvent),
	G4UserEventAction()
{
		if (fEvent.GetSimData().GetSimulationMode() == SimData::SimulationMode::eFast) {
			cout << "[INFO] G4HodoscopeSimulator::G4HodoscopeEventAction: Running Simulation in Fast mode" << endl;
			string fileName = theEvent.GetSimData().GetOutputFileName();
			cout << "[INFO] G4HodoscopeSimulator::G4HodoscopeEventAction: opening outputfile: " << fileName << endl;
			fOutFile = std::ofstream(fileName, ofstream::out | ofstream::app);
		
	}

}

G4HodoscopeEventAction::~G4HodoscopeEventAction()
{
		if (fEvent.GetSimData().GetSimulationMode() == SimData::SimulationMode::eFast)
			fOutFile.close();
}

void
G4HodoscopeEventAction::BeginOfEventAction(const G4Event*)
{
	trigX1 = false;
	trigY1 = false;
	trig1 = false;
	
	fBarsX1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	fBarsY1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	trigX2 = false;
	trigY2 = false;
	trig2 = false;
	
	fBarsX2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	fBarsY2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	trigX3 = false;
	trigY3 = false;
	trig3 = false;
	
	fBarsX3 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	fBarsY3 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

}

void 
G4HodoscopeEventAction::EndOfEventAction(const G4Event*)
{


	// compute matrix of hits for each panel

	if (trigX1 & trigY1)
		trig1 = true;

	if (trigX2 & trigY2)
		trig2 = true;

	if (trigX3 & trigY3)
		trig3 = true;

	// print hits if there was coincidence between 3 panels
	if (trig1 & trig2 & trig3){

		for (auto const& val : fBarsX1)
			fOutFile << val << " ";
	
		for (auto const& val : fBarsY1)
			fOutFile << val << " ";
	
		for (auto const& val : fBarsX2)
			fOutFile << val << " ";
	
		for (auto const& val : fBarsY2)
			fOutFile << val << " ";

		for (auto const& val : fBarsX3)
			fOutFile << val << " ";
	
		for (auto const& val : fBarsY3)
			fOutFile << val << " ";
		
		fOutFile << endl;
	
	}

	
}

