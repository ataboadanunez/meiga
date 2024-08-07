/* 
	
	Application for simulating a muon telescope

	author: alvaro taboada
	date: 23 Jan 2023

 */

// Headers of this particular application
#include "G4LeadSimulator.h"
#include "G4LeadDetectorConstruction.h"
#include "G4MPrimaryGeneratorAction.h"
#include "G4LeadEventAction.h"
#include "G4LeadRunAction.h"
#include "G4LeadTrackingAction.h"
#include "G4LeadSteppingAction.h"
#include "G4LeadPhysicsList.h" 

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

using namespace std;

Particle G4LeadSimulator::currentParticle;
G4LeadSimulator* fG4LeadSimulator;
string fCfgFile;

G4LeadSimulator::G4LeadSimulator()
{
}

G4LeadSimulator::~G4LeadSimulator()
{
}

namespace 
{
	void ProgramUsage() 
	{
		cerr << " Program Usage: " << endl;
		cerr << " ./G4LeadSimulator [ -c ConfigFile.json ] " << endl;
	}

}


int main(int argc, char** argv) 
{

	if (argc < 3) {
		ProgramUsage();
		throw invalid_argument("[ERROR] G4LeadSimulator::main: A configuration file is needed!");
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
	return 0;
}

void
G4LeadSimulator::Initialize(Event& theEvent, string cfgFile)
{
	cout << "[INFO] G4LeadSimulator::Initialize" << endl;
	cout << "[INFO] G4LeadSimulator::Initialize: Reading configuration file " << cfgFile << endl;
	// Fill Event object from configuration file
	// Read Simulation configuration
	theEvent = ConfigManager::ReadConfigurationFile(cfgFile);
	// extra flag to handle lead brick simulation
	ptree tree;
	read_json(cfgFile, tree);
	fSimulateBrick = tree.get<bool>("LeadBrick.Simulate", false);
	// get simulation simulation settings
	const Event::Config &cfg = theEvent.GetConfig();
	ConfigManager::PrintConfig(cfg);
	// Read Detector Configuration
	ConfigManager::ReadDetectorList(cfg.fDetectorList, theEvent);
}            


bool
G4LeadSimulator::RunSimulation(Event& theEvent)
{
	cout << "[INFO] G4LeadSimulator::RunSimulation" << endl;
	const Event::Config &cfg = theEvent.GetConfig();
	SimData& simData = theEvent.GetSimData();
	const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();
	cout << "[INFO] G4LeadSimulator::RunSimulation: Number of particles to be simulated = " << NumberOfParticles << endl;
	if (!NumberOfParticles) {
		cerr << "[ERROR] G4LeadSimulator::RunSimulation: No Particles in the Event! Exiting." << endl;
		return false;
	}
	/***************
		Geant4 Setup    
	*****************/
	G4long myseed = time(NULL);
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(myseed);
	cout << "Seed for random generation: " << myseed << endl;
	// construct the default run manager
	G4VisManager *visManager = nullptr;
	G4RunManager *runManager = G4RunManagerFactory::CreateRunManager();
	// set mandatory initialization classes
	auto fDetConstruction = new G4LeadDetectorConstruction(theEvent, fSimulateBrick);
	runManager->SetUserInitialization(fDetConstruction);
	runManager->SetUserInitialization(new G4LeadPhysicsList(cfg.fPhysicsListName));  
	G4MPrimaryGeneratorAction *fPrimaryGenerator = new G4MPrimaryGeneratorAction(theEvent);
	runManager->SetUserAction(fPrimaryGenerator);
	G4LeadRunAction *fRunAction = new G4LeadRunAction(theEvent);
	runManager->SetUserAction(fRunAction);
	G4LeadEventAction *fEventAction = new G4LeadEventAction(theEvent);
	runManager->SetUserAction(fEventAction);
	runManager->SetUserAction(new G4LeadTrackingAction(theEvent));
	G4LeadSteppingAction *fSteppingAction = new G4LeadSteppingAction(theEvent);
	runManager->SetUserAction(fSteppingAction);
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
		uiManager->ApplyCommand(("/vis/open " + fRenderFile).c_str());
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
		G4LeadSimulator::currentParticle = *it;
		simData.SetCurrentParticle(*it);
		// Run simulation
		runManager->BeamOn(1);
	}
	delete runManager;
	delete visManager;
	cout << "[INFO] G4LeadSimulator::RunSimulation: Geant4 Simulation ended successfully. " << endl;
	return true;
}


void
G4LeadSimulator::WriteEventInfo(Event& theEvent)
{
	cout << "[INFO] G4LeadSimulator::WriteEventInfo" << endl;
	DataWriter::FileWriter(theEvent);

	return;
}
