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

Particle G4LeadSimulator::currentParticle;
G4LeadSimulator* fG4LeadSimulator;
string fCfgFile;
const string cApplicationName = "G4LeadSimulator";

G4LeadSimulator::G4LeadSimulator()
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
	fG4LeadSimulator = new G4LeadSimulator();
	// Create Event object
	Event theEvent;
	fG4LeadSimulator->Initialize(theEvent, fCfgFile);
	fG4LeadSimulator->RunSimulation(theEvent);
	/*************************************************
		Geant4 simulation ended here!
		What happens next is up to you =)
	**************************************************/
	fG4LeadSimulator->WriteEventInfo(theEvent);
	time(&end);
	
	// Calculating total time taken by the program.
	double time_taken = double(end - start);
	cout << "[INFO] G4LeadSimulator: Time taken by program is : " << fixed
			 << time_taken << setprecision(5);
	cout << " sec " << endl;
	
	delete fG4LeadSimulator;
	return 0;
}

bool
G4LeadSimulator::RunSimulation(Event &aEvent)
{
	cout << "[INFO] G4LeadSimulator::RunSimulation" << endl;
	const Event::Config &cfg = aEvent.GetConfig();
	SimData& simData = aEvent.GetSimData();
	const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();
	cout << "[INFO] G4LeadSimulator::RunSimulation: Number of particles to be simulated = " << NumberOfParticles << endl;
	if (!NumberOfParticles) {
		cerr << "[ERROR] G4LeadSimulator::RunSimulation: No Particles in the Event! Exiting." << endl;
		return false;
	}
	
	G4long myseed = time(NULL);
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(myseed);

	// construct the default run manager
	auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
	// set mandatory initialization classes
	runManager->SetUserInitialization(new G4LeadDetectorConstruction(aEvent));
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
	
	cout << "[INFO] G4LeadSimulator::RunSimulation: Geant4 Simulation ended successfully. " << endl;
	return true;
}