/* 
	
	Application for simulating the muon decay in a WCD

	author: alvaro taboada
	date: 9 Feb 2022

 */

// Headers of this particular application
#include "G4MuDecSimulator.h"
#include "G4MuDecConstruction.h"
#include "G4MuDecActionInitialization.h"

// Geant4 headers
#include "FTFP_BERT.hh"
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"

// Framework libraries
#include "ConfigManager.h"
#include "CorsikaUtilities.h"
#include "Event.h"
#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"
#include "OptDevice.h"
#include "G4MPhysicsList.h"
#include "DataWriter.h"
#include "Logger.h"

using namespace std;

Particle G4MuDecSimulator::currentParticle;
G4MuDecSimulator* fG4MuDecSimulator;
string fCfgFile;
const string cApplicationName = "G4MuDecSimulator";

G4MuDecSimulator::G4MuDecSimulator()
{

}

int main(int argc, char** argv) 
{

	DisplayLogo();
	if (argc < 3) {
		ProgramUsage(cApplicationName);
		throw invalid_argument("[ERROR] A configuration file is needed!");
	}

	for (int i=1; i<argc; i=i+2) {
		string sarg(argv[i]);
		if (sarg == "-c")
			fCfgFile = argv[i+1];
	}

	// for program time calculation
	time_t start, end;
	time(&start);

	fG4MuDecSimulator = new G4MuDecSimulator();
	// Create Event object
	Event theEvent;
	fG4MuDecSimulator->Initialize(theEvent, fCfgFile);
	fG4MuDecSimulator->RunSimulation(theEvent);
	/*************************************************
		
		Geant4 simulation ended here!
		What happens next is up to you =)

	**************************************************/
	fG4MuDecSimulator->WriteEventInfo(theEvent);
	time(&end);

	// Calculating total time taken by the program.
	double time_taken = double(end - start);
	cout << "[INFO] G4MuDecSimulator: Time taken by program is : " << fixed
			 << time_taken << setprecision(5);
	cout << " sec " << endl;
	
	return 0;

}

bool
G4MuDecSimulator::RunSimulation(Event& aEvent)
{

	cout << "[INFO] G4MuDecSimulator::RunSimulation" << endl;
	
	const Event::Config &cfg = aEvent.GetConfig();
	SimData& simData = aEvent.GetSimData();
	const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();
	cout << "[INFO] G4MuDecSimulator::RunSimulation: Number of particles to be simulated = " << NumberOfParticles << endl;
	
	if (!NumberOfParticles) {
		cerr << "[ERROR] G4MuDecSimulator::RunSimulation: No Particles in the Event! Exiting." << endl;
		return false;
	}

	G4long myseed = time(NULL);
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(myseed);

	G4VisManager* visManager = nullptr;
	
	// construct the default run manager
	auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);

	// set mandatory initialization classes
	auto fDetConstruction = new G4MuDecConstruction(aEvent);
	runManager->SetUserInitialization(fDetConstruction);
	runManager->SetUserInitialization(new G4MPhysicsList(cfg.fPhysicsListName));
	runManager->SetUserInitialization(new G4MuDecActionInitialization(aEvent));

	// initialize G4 kernel
	runManager->Initialize();

	// initialize visualization
	if ((cfg.fGeoVis || cfg.fTrajVis) && !visManager)
		visManager = new G4VisExecutive;

	// get the pointer to the UI manager and set verbosities
	G4UImanager* uiManager = G4UImanager::GetUIpointer();
	switch (cfg.fVerbosity) {
		case 1:
			uiManager->ApplyCommand("/run/verbose 1");
			uiManager->ApplyCommand("/event/verbose 0");
			uiManager->ApplyCommand("/tracking/verbose 0");
			break;
		case 2:
			uiManager->ApplyCommand("/run/verbose 1");
			uiManager->ApplyCommand("/event/verbose 1");
			uiManager->ApplyCommand("/tracking/verbose 0");
			break;
		case 3:
			uiManager->ApplyCommand("/run/verbose 1");
			uiManager->ApplyCommand("/event/verbose 1");
			uiManager->ApplyCommand("/tracking/verbose 1");
			break;
		default:
			uiManager->ApplyCommand("/run/verbose 0");
			uiManager->ApplyCommand("/event/verbose 0");
			uiManager->ApplyCommand("/tracking/verbose 0");
		}
	
	if (cfg.fGeoVis || cfg.fTrajVis) {
		visManager->Initialize();
		uiManager->ApplyCommand(("/vis/open " + cfg.fRenderFile).c_str());
		uiManager->ApplyCommand("/vis/scene/create");
		uiManager->ApplyCommand("/vis/sceneHandler/attach");
		uiManager->ApplyCommand("/vis/scene/add/volume");
		uiManager->ApplyCommand("/vis/scene/add/axes 0 0 0 1 m");
		uiManager->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 0. 0.");
		uiManager->ApplyCommand("/vis/viewer/set/targetPoint 0 0 0");
		uiManager->ApplyCommand("/vis/viewer/zoom 1");
		uiManager->ApplyCommand("/vis/viewero/set/style/wireframe");
		uiManager->ApplyCommand("/vis/drawVolume");
		uiManager->ApplyCommand("/vis/scene/notifyHandlers");
		uiManager->ApplyCommand("/vis/viewer/update");

	}

	if (cfg.fTrajVis) {
			uiManager->ApplyCommand("/tracking/storeTrajectory 1");
			uiManager->ApplyCommand("/vis/scene/add/trajectories");
	}
	

	// loop over particle vector
	for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
		G4MuDecSimulator::currentParticle = *it;
		simData.SetCurrentParticle(*it);
		// Run simulation
		runManager->BeamOn(1);
	}


	delete visManager;
	delete runManager;

	cout << "[INFO] G4MuDecSimulator::RunSimulation: Geant4 Simulation ended successfully. " << endl;

	return true;

}