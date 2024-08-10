/* 
	
	Application for simulating a muon telescope

	author: alvaro taboada
	date: 23 Jan 2023

 */

// Headers of this particular application
#include "G4LeadSimulator.h"
#include "G4LeadDetectorConstruction.h"
#include "G4LeadActionInitialization.h"

// Geant4 headers
#include "FTFP_BERT.hh"
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

Particle G4LeadSimulator::currentParticle;
G4LeadSimulator* fG4LeadSimulator;
string fCfgFile;
const string cApplicationName = "G4LeadSimulator";

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

	fG4LeadSimulator = new G4LeadSimulator();
	// Create Event object
	Event theEvent;
	fG4LeadSimulator->Initialize(theEvent, fCfgFile);
	if(fG4LeadSimulator->RunSimulation(theEvent)) {
		fG4LeadSimulator->WriteEventInfo(theEvent);
	}
	timer.PrintElapsedTime();
	
	delete fG4LeadSimulator;
	return 0;
}

G4LeadSimulator::G4LeadSimulator()
{
}

void G4LeadSimulator::Initialize(Event &aEvent, std::string aFileName)
{
	// Fill Event object from configuration file
	ConfigManager::ReadConfigurationFile(aEvent, aFileName);
	// get simulation simulation settings
	const Event::Config &cfg = aEvent.GetConfig();
	ConfigManager::PrintConfig(cfg);
	// Read Detector Configuration
	ConfigManager::ReadDetectorList(cfg.fDetectorList, aEvent);
	
	// extra flag to handle lead brick simulation
	ptree tree;
	read_json(aFileName, tree);
	fSimulateBrick = tree.get<bool>("LeadBrick.Simulate", false);
}

bool G4LeadSimulator::RunSimulation(Event &aEvent)
{
	const Event::Config &cfg = aEvent.GetConfig();
	SimData& simData = aEvent.GetSimData();
	const unsigned int numberOfParticles = simData.GetTotalNumberOfParticles();
	ostringstream msg;
	if (!numberOfParticles) {
		msg << "No particles in the Event! Exiting...";
		Logger::Print(msg, ERROR, "RunSimulation");	
		return false;
	}
	msg << "Number of particles to be simulated: " << numberOfParticles;
	Logger::Print(msg, INFO, "RunSimulation");
	
	G4long myseed = time(NULL);
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(myseed);

	// construct the default run manager
	auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
	// set mandatory initialization classes
	runManager->SetUserInitialization(new G4LeadDetectorConstruction(aEvent, fSimulateBrick));
	runManager->SetUserInitialization(new G4MPhysicsList(cfg.fPhysicsListName));
	runManager->SetUserInitialization(new G4LeadActionInitialization(aEvent));
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
	
	// loop over particle vector
	for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
		G4LeadSimulator::currentParticle = *it;
		simData.SetCurrentParticle(*it);
		// Run simulation
		runManager->BeamOn(1);
	}
	
	delete visManager;
	delete runManager;
	
	Logger::Print("Geant4 Simulation ended successfully.", INFO, "RunSimulation");
	return true;
}
