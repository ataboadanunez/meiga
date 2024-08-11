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

	Logger::DisplayLogo();
	try {
		Logger::CheckCommandLineParameters(argc, argv, fCfgFile, cApplicationName);
	} catch (const std::invalid_argument &e) {
		Logger::Print(e.what(), ERROR, cApplicationName);
		Logger::ProgramUsage(cApplicationName);
		return 1;
	}

	Timer timer;

	fG4MuDecSimulator = new G4MuDecSimulator();
	// Create Event object
	Event theEvent;
	fG4MuDecSimulator->Initialize(theEvent, fCfgFile);
	if(fG4MuDecSimulator->RunSimulation(theEvent)) {
		fG4MuDecSimulator->WriteEventInfo(theEvent);
	}
	timer.PrintElapsedTime();
	
	delete fG4MuDecSimulator;
	return 0;

}

bool
G4MuDecSimulator::RunSimulation(Event& aEvent)
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
	auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
	// set mandatory initialization classes
	auto fDetConstruction = new G4MuDecConstruction(aEvent);
	runManager->SetUserInitialization(fDetConstruction);
	runManager->SetUserInitialization(new G4MPhysicsList(cfg.fPhysicsListName));
	runManager->SetUserInitialization(new G4MuDecActionInitialization(aEvent));
	// initialize G4 kernel
	runManager->Initialize();

	// setup ui & visualization managers
	G4UImanager* uiManager = G4UImanager::GetUIpointer();
	G4VisManager* visManager = new G4VisExecutive;;
	SetupManagers(aEvent, *uiManager, *visManager);

	// loop over particle vector
	for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
		G4MuDecSimulator::currentParticle = *it;
		simData.SetCurrentParticle(*it);
		// Run simulation
		runManager->BeamOn(1);
	}
	
	delete visManager;
	delete runManager;

	Logger::Print("Geant4 Simulation ended successfully.", INFO, "RunSimulation");
	return true;

}