/* 
	
	Application for simulating a muon telescope

	author: alvaro taboada
	date: 23 Jan 2023

 */

// Headers of this particular application
#include "G4HodoscopeSimulator.h"
#include "G4HodoscopeDetectorConstruction.h"
#include "G4HodoscopeActionInitialization.h"

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

Particle G4HodoscopeSimulator::currentParticle;
G4HodoscopeSimulator* fG4HodoscopeSimulator;
string fCfgFile;
const string cApplicationName = "G4HodoscopeSimulator";

G4HodoscopeSimulator::G4HodoscopeSimulator()
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
	fG4HodoscopeSimulator = new G4HodoscopeSimulator();
	// Create Event object
	Event theEvent;
	fG4HodoscopeSimulator->Initialize(theEvent, fCfgFile);
	fG4HodoscopeSimulator->RunSimulation(theEvent);
	/*************************************************
		Geant4 simulation ended here!
		What happens next is up to you =)
	**************************************************/
	fG4HodoscopeSimulator->WriteEventInfo(theEvent);
	time(&end);
	// Calculating total time taken by the program.
	double time_taken = double(end - start);
	cout << "[INFO] G4HodoscopeSimulator: Time taken by program is : " << fixed
			 << time_taken << setprecision(5);
	cout << " sec " << endl;
	return 0;
}

bool
G4HodoscopeSimulator::RunSimulation(Event& theEvent)
{
	cout << "[INFO] G4HodoscopeSimulator::RunSimulation" << endl;
	const Event::Config &cfg = theEvent.GetConfig();
	SimData& simData = theEvent.GetSimData();
	const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();
	cout << "[INFO] G4HodoscopeSimulator::RunSimulation: Number of particles to be simulated = " << NumberOfParticles << endl;
	if (!NumberOfParticles) {
		cerr << "[ERROR] G4HodoscopeSimulator::RunSimulation: No Particles in the Event! Exiting." << endl;
		return false;
	}
	
	G4long myseed = time(NULL);
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(myseed);
	
	G4VisManager* visManager = nullptr;
	// construct the default run manager
	auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
	// set mandatory initialization classes
	runManager->SetUserInitialization(new G4HodoscopeDetectorConstruction(theEvent));
	runManager->SetUserInitialization(new G4MPhysicsList(cfg.fPhysicsListName));
	runManager->SetUserInitialization(new G4HodoscopeActionInitialization(theEvent));
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
		uiManager->ApplyCommand("/vis/scene/add/axes");
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
		uiManager->ApplyCommand("/vis/filtering/trajectories/create/particleFilter");
		// for debugging purposes, gammas are not drawn
		uiManager->ApplyCommand("/vis/filtering/trajectories/particleFilter-0/add opticalphoton");
		uiManager->ApplyCommand("/vis/filtering/trajectories/particleFilter-0/invert true");
	}
	// loop over particle vector
	for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
		G4HodoscopeSimulator::currentParticle = *it;
		simData.SetCurrentParticle(*it);
		// Run simulation
		runManager->BeamOn(1);
	}
	delete visManager;
	delete runManager;
	cout << "[INFO] G4HodoscopeSimulator::RunSimulation: Geant4 Simulation ended successfully. " << endl;
	return true;
}