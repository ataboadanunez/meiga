/* 
	
	Main script of an example application using Geant4

	author: alvaro taboada
	date: 15 Sep 2021

	$Id:$

 */

// Headers of this particular application
#include "G4ExSimulator.h"
#include "G4ExDetectorConstruction.h"
#include "G4ExActionInitialization.h"

// Geant4 headers
#include "FTFP_BERT.hh"
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"
#include "G4SDManager.hh"
#include "G4AutoDelete.hh"

// Framework libraries
#include "ConfigManager.h"
#include "CorsikaUtilities.h"
#include "Event.h"
#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"
#include "OptDevice.h"
#include "G4MPhysicsList.h"
#include "G4MPrimaryGeneratorAction.h"
#include "DataWriter.h"

using namespace std;

Particle G4ExSimulator::currentParticle;
G4ExSimulator* fG4ExSimulator;
string fCfgFile;

G4ExSimulator::G4ExSimulator()
{

}

namespace 
{
	void ProgramUsage() 
	{
		cerr << " Program Usage: " << endl;
		cerr << " ./G4ExSimulator [ -c ConfigFile.json ] " << endl;
	}

}

int main(int argc, char** argv) 
{

	if (argc < 3) {
		ProgramUsage();
		throw invalid_argument("[ERROR] G4ExSimulator::main: A configuration file is needed!");
	}

	for (int i=1; i<argc; i=i+2) {
		string sarg(argv[i]);
		if (sarg == "-c")
			fCfgFile = argv[i+1];
	}
	
	// for program time calculation
  	time_t start, end;
  	time(&start);
  
	fG4ExSimulator = new G4ExSimulator();
	// Initialize Event object
	Event theEvent;
	fG4ExSimulator->Initialize(theEvent, fCfgFile);
	fG4ExSimulator->RunSimulation(theEvent);
	/*************************************************
		
		Geant4 simulation ended here!
		What happens next is up to you =)

	**************************************************/
	fG4ExSimulator->WriteEventInfo(theEvent);
	time(&end);

	// Calculating total time taken by the program.
    double time_taken = double(end - start);
    cout << "[INFO] G4ExSimulator: Time taken by program is : " << fixed
         << time_taken << setprecision(5);
    cout << " sec " << endl;
    
	delete fG4ExSimulator;
	return 0;

}

void
G4ExSimulator::Initialize(Event &aEvent, string aFileName)
{
	// Fill Event object from configuration file
	ConfigManager::ReadConfigurationFile(aEvent, aFileName);
	// get simulation simulation settings
	const Event::Config &cfg = aEvent.GetConfig();
	ConfigManager::PrintConfig(cfg);
	// Read Detector Configuration
	ConfigManager::ReadDetectorList(cfg.fDetectorList, aEvent);
}            


bool
G4ExSimulator::RunSimulation(Event& aEvent)
{
	const Event::Config &cfg = aEvent.GetConfig();
	SimData& simData = aEvent.GetSimData();
	const unsigned int numberOfParticles = simData.GetTotalNumberOfParticles();
	cout << "[INFO] G4ExSimulator::RunSimulation: Number of particles to be simulated = " << numberOfParticles << endl;
	if (!numberOfParticles) {
		cerr << "[ERROR] G4ExSimulator::RunSimulation: No Particles in the Event! Exiting..." << endl;
		return false;
	}
	
	G4long myseed = time(NULL);
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(myseed);

	// TODO: move lines below to a function.
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
	
	// initialize visualization
	G4VisManager* visManager = nullptr;
	if ((cfg.fGeoVis || cfg.fTrajVis) && !visManager) {
		visManager = new G4VisExecutive;
	}
	if (visManager) {
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
		uiManager->ApplyCommand("/vis/filtering/trajectories/create/particleFilter");
		// for debugging purposes, gammas are not drawn
		uiManager->ApplyCommand("/vis/filtering/trajectories/particleFilter-0/add opticalphoton");
		uiManager->ApplyCommand("/vis/filtering/trajectories/particleFilter-0/invert true");
	}

	// construct the default run manager
	auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
	// set mandatory initialization classes
	auto fDetConstruction = new G4ExDetectorConstruction(aEvent);
	runManager->SetUserInitialization(fDetConstruction);
	runManager->SetUserInitialization(new G4MPhysicsList(cfg.fPhysicsListName));
	runManager->SetUserInitialization(new G4ExActionInitialization(aEvent));
	// initialize G4 kernel
	runManager->Initialize();

	// loop over particle vector
	for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
		G4ExSimulator::currentParticle = *it;
		simData.SetCurrentParticle(*it);
		// Run simulation
		runManager->BeamOn(1);
	}


	delete visManager;
	delete runManager;

	cout << "[INFO] G4ExSimulator::RunSimulation: Geant4 Simulation ended successfully. " << endl;
	
	return true;

}


void
G4ExSimulator::WriteEventInfo(Event& aEvent)
{
	cout << "[INFO] G4ExSimulator::WriteEventInfo" << endl;
DataWriter::FileWriter(aEvent);

	return;

}
