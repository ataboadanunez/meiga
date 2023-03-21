// implementation of the G4TandarSteppingAction class
#include "G4TandarSimulator.h"
#include "G4TandarSteppingAction.h"
#include "G4TandarDetectorConstruction.h"
#include "G4TandarEventAction.h"
#include "G4TandarTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"

#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"

#include <G4OpticalPhoton.hh>

using namespace std;

G4TandarSteppingAction::G4TandarSteppingAction(G4TandarEventAction* G4event, Event& theEvent)
	: G4UserSteppingAction(),
		fEventAction(G4event),
    fEvent(theEvent)
{


	if (fEvent.GetSimData().GetSimulationMode() == SimData::SimulationMode::eFast) {
		cout << "[INFO] G4TandarSimulator::G4TandarSteppingAction: Running Simulation in Fast mode." << endl;
		cout << "[INFO] G4TandarSimulator::G4TandarSteppingAction: Optical photons will not be propagated!" << endl;
		
	}

}

G4TandarSteppingAction::~G4TandarSteppingAction()
{

}

void
G4TandarSteppingAction::UserSteppingAction(const G4Step* step)
{

	int pX_1 = 0;
	int pY_1 = 0;
	int pX_2 = 0;
	int pY_2 = 0;
	int pX_3 = 0;
	int pY_3 = 0;


	G4Track *track = step->GetTrack();
	SimData &simData = fEvent.GetSimData();
	if (simData.GetSimulationMode() == SimData::SimulationMode::eFast) {
		
		// if fast mode activated, kill optical photons and skip secondary particles
		
		if (track->GetParticleDefinition() == G4OpticalPhoton::OpticalPhoton())
			track->SetTrackStatus(fStopAndKill);

		if (track->GetTrackID() != 1) 
			return;

		string volName = track->GetVolume()->GetName();
		int volId = track->GetVolume()->GetCopyNo();

		// get panel id from volume name
		string volIdstr = to_string(volId);
		char panelId = volIdstr[0];

		if ((panelId == '1') & (volName == "BarScin_X")) {
			
			if (fEventAction->trigX1)
				return;

			//string volIdstr = to_string(volId);
			const char *volIdchar = volIdstr.c_str();
			//char panelId = volIdchar[0];
			char barId0 = volIdchar[1];
			char barId1 = volIdchar[2];
			// magic conversion to barId
			string barIdStr = string(1, barId0) + barId1;
			// now get the integer bar ID again
			int barId = stoi(barIdStr);
			pX_1 = barId;
			fEventAction->fBarsX1.at(pX_1-1) += 1;
			fEventAction->trigX1 = true;


		} else if ((panelId == '1') & (volName == "BarScin_Y")) {

			if (fEventAction->trigY1)
				return;

			//string volIdstr = to_string(volId);
			const char *volIdchar = volIdstr.c_str();
			//char panelId = volIdchar[0];
			char barId0 = volIdchar[1];
			char barId1 = volIdchar[2];
			// magic conversion to barId
			string barIdStr = string(1, barId0) + barId1;
			// now get the integer bar ID again
			int barId = stoi(barIdStr);
			pY_1 = barId - 12;
			fEventAction->fBarsY1.at(pY_1-1) += 1;
			fEventAction->trigY1 = true;

		}

		else if ((panelId == '2') & (volName == "BarScin_X")) {
			
			if (fEventAction->trigX2)
				return;

			//string volIdstr = to_string(volId);
			const char *volIdchar = volIdstr.c_str();
			//char panelId = volIdchar[0];
			char barId0 = volIdchar[1];
			char barId1 = volIdchar[2];
			// magic conversion to barId
			string barIdStr = string(1, barId0) + barId1;
			// now get the integer bar ID again
			int barId = stoi(barIdStr);
			pX_2 = barId;
			fEventAction->fBarsX2.at(pX_2-1) += 1;
			fEventAction->trigX2 = true;


		} else if ((panelId == '2') & (volName == "BarScin_Y")) {

			if (fEventAction->trigY2)
				return;

			//string volIdstr = to_string(volId);
			const char *volIdchar = volIdstr.c_str();
			//char panelId = volIdchar[0];
			char barId0 = volIdchar[1];
			char barId1 = volIdchar[2];
			// magic conversion to barId
			string barIdStr = string(1, barId0) + barId1;
			// now get the integer bar ID again
			int barId = stoi(barIdStr);
			pY_2 = barId - 12;
			fEventAction->fBarsY2.at(pY_2-1) += 1;
			fEventAction->trigY2 = true;

		}

		else if ((panelId == '3') & (volName == "BarScin_X")) {
			
			if (fEventAction->trigX3)
				return;

			//string volIdstr = to_string(volId);
			const char *volIdchar = volIdstr.c_str();
			//char panelId = volIdchar[0];
			char barId0 = volIdchar[1];
			char barId1 = volIdchar[2];
			// magic conversion to barId
			string barIdStr = string(1, barId0) + barId1;
			// now get the integer bar ID again
			int barId = stoi(barIdStr);
			pX_3 = barId;
			fEventAction->fBarsX3.at(pX_3-1) += 1;
			fEventAction->trigX3 = true;


		} else if ((panelId == '3') & (volName == "BarScin_Y")) {

			if (fEventAction->trigY3)
				return;

			// string volIdstr = to_string(volId);
			const char *volIdchar = volIdstr.c_str();
			//char panelId = volIdchar[0];
			char barId0 = volIdchar[1];
			char barId1 = volIdchar[2];
			// magic conversion to barId
			string barIdStr = string(1, barId0) + barId1;
			// now get the integer bar ID again
			int barId = stoi(barIdStr);
			pY_3 = barId - 12;
			fEventAction->fBarsY3.at(pY_3-1) += 1;
			fEventAction->trigY3 = true;

		}

		else
			return;
	}

	

}