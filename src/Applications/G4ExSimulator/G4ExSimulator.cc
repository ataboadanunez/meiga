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
#include "Logger.h"
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
const string cApplicationName = "G4ExSimulator";

G4ExSimulator::G4ExSimulator()
{
}

int main(int argc, char** argv) 
{
	Logger::DisplayLogo();
	try {
		Logger::CheckCommandLineParameters(argc, argv, fCfgFile, cApplicationName);
	} catch (const std::invalid_argument &e) {
		Logger::Print(e.what(), ERROR, cApplicationName);
		Logger::ProgramUsage(cApplicationName);
		return 1;
	}
	
	Timer timer;

	fG4ExSimulator = new G4ExSimulator();
	// Initialize Event object
	Event theEvent;
	fG4ExSimulator->Initialize(theEvent, fCfgFile);
	if(fG4ExSimulator->RunSimulation(theEvent)) {
		fG4ExSimulator->WriteEventInfo(theEvent);
	}
	timer.PrintElapsedTime();
	
	delete fG4ExSimulator;
	return 0;
}

bool
G4ExSimulator::RunSimulation(Event& aEvent)
{
	const Event::Config &cfg = aEvent.GetConfig();
	SimData& simData = aEvent.GetSimData();
	const unsigned int numberOfParticles = simData.GetTotalNumberOfParticles();
	ostringstream msg;
	if (!numberOfParticles && simData.GetInputMode() != SimData::InputMode::eGPS) {
		msg << "No particles in the Event! Exiting...";
		Logger::Print(msg, ERROR, "RunSimulation");	
		return false;
	} else {
		msg << "Number of particles to be simulated: " << numberOfParticles;
		Logger::Print(msg, INFO, "RunSimulation");
	}
	
	G4long myseed = time(NULL);
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(myseed);

	G4RunManager* runManager = nullptr;
	// construct the default run manager
	if (simData.GetInputMode() == SimData::InputMode::eGPS) {
		// testing MT
		runManager = G4RunManagerFactory::CreateRunManager();
		runManager->SetNumberOfThreads(1);
	} else {
		runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
	}
	// set mandatory initialization classes
	auto fDetConstruction = new G4ExDetectorConstruction(aEvent);
	runManager->SetUserInitialization(fDetConstruction);
	runManager->SetUserInitialization(new G4MPhysicsList(cfg.fPhysicsListName));
	runManager->SetUserInitialization(new G4ExActionInitialization(aEvent));
	// initialize G4 kernel
	runManager->Initialize();

	// setup ui & visualization managers
	G4UImanager* uiManager = G4UImanager::GetUIpointer();
	G4VisManager* visManager = new G4VisExecutive;
	SetupManagers(aEvent, *uiManager, *visManager);
	// for debugging purposes, gammas are not draw:
	uiManager->ApplyCommand("/vis/filtering/trajectories/create/particleFilter");
	uiManager->ApplyCommand("/vis/filtering/trajectories/particleFilter-0/add opticalphoton");
	uiManager->ApplyCommand("/vis/filtering/trajectories/particleFilter-0/invert true");
	
	if (simData.GetInputMode() == SimData::InputMode::eGPS) {
		uiManager->ApplyCommand("/control/execute " + cfg.fInputFileName);
	} else {
		for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
			G4ExSimulator::currentParticle = *it;
			simData.SetCurrentParticle(*it);
			runManager->BeamOn(1);
		}
	}

	delete visManager;
	delete runManager;
	
	Logger::Print("Geant4 Simulation ended successfully.", INFO, "RunSimulation");
	return true;

}