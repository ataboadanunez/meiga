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

	Logger::DisplayLogo();
	try {
		Logger::CheckCommandLineParameters(argc, argv, fCfgFile, cApplicationName);
	} catch (const std::invalid_argument &e) {
		Logger::Print(e.what(), ERROR, cApplicationName);
		Logger::ProgramUsage(cApplicationName);
		return 1;
	}
	
	Timer timer;

	fG4HodoscopeSimulator = new G4HodoscopeSimulator();
	// Initialize Event object
	Event theEvent;
	fG4HodoscopeSimulator->Initialize(theEvent, fCfgFile);
	if(fG4HodoscopeSimulator->RunSimulation(theEvent)) {
		fG4HodoscopeSimulator->WriteEventInfo(theEvent);
	}
	timer.PrintElapsedTime();
	
	delete fG4HodoscopeSimulator;
	return 0;
}

bool
G4HodoscopeSimulator::RunSimulation(Event& aEvent)
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
	
	// construct the default run manager
	auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
	// set mandatory initialization classes
	runManager->SetUserInitialization(new G4HodoscopeDetectorConstruction(aEvent));
	runManager->SetUserInitialization(new G4MPhysicsList(cfg.fPhysicsListName));
	runManager->SetUserInitialization(new G4HodoscopeActionInitialization(aEvent));
	// initialize G4 kernel
	runManager->Initialize();
	
	// setup ui & visualization managers
	G4UImanager* uiManager = G4UImanager::GetUIpointer();
	G4VisManager* visManager = new G4VisExecutive;;
	SetupManagers(aEvent, *uiManager, *visManager);
	// for debugging purposes, gammas are not draw:
	uiManager->ApplyCommand("/vis/filtering/trajectories/create/particleFilter");
	uiManager->ApplyCommand("/vis/filtering/trajectories/particleFilter-0/add opticalphoton");
	uiManager->ApplyCommand("/vis/filtering/trajectories/particleFilter-0/invert true");

	if (simData.GetInputMode() == SimData::InputMode::eGPS) {
		uiManager->ApplyCommand("/control/execute " + cfg.fInputFileName);
	} else {
		for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
			G4HodoscopeSimulator::currentParticle = *it;
			simData.SetCurrentParticle(*it);
			runManager->BeamOn(1);
		}
	}
	
	delete visManager;
	delete runManager;
	
	Logger::Print("Geant4 Simulation ended successfully.", INFO, "RunSimulation");
	return true;
}