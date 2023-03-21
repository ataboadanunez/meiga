/* 
	
	Application for simulating Muography experiment at
	Mina Casposo, San Juan, Argentina

	author: alvaro taboada
	date: 6 May 2022

 */

// Headers of this particular application
#include "G4TandarSimulator.h"
#include "G4TandarDetectorConstruction.h"
#include "G4MPrimaryGeneratorAction.h"
#include "G4TandarEventAction.h"
#include "G4TandarRunAction.h"
#include "G4TandarTrackingAction.h"
#include "G4TandarSteppingAction.h"
#include "G4TandarPhysicsList.h" 

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

using namespace std;

Particle G4TandarSimulator::currentParticle;
G4TandarSimulator* fG4TandarSimulator;
string fCfgFile;

G4TandarSimulator::G4TandarSimulator()
{

}

namespace 
{
	void ProgramUsage() 
	{
		cerr << " Program Usage: " << endl;
		cerr << " ./G4TandarSimulator [ -c ConfigFile.json ] " << endl;
	}

}


int main(int argc, char** argv) 
{

	if (argc < 3) {
		ProgramUsage();
		throw invalid_argument("[ERROR] G4TandarSimulator::main: A configuration file is needed!");
	}

	for (int i=1; i<argc; i=i+2) {
		string sarg(argv[i]);
		if (sarg == "-c")
			fCfgFile = argv[i+1];
	}

	// for program time calculation
	time_t start, end;
	time(&start);

	fG4TandarSimulator = new G4TandarSimulator();
	// Create Event object
	Event theEvent;
	fG4TandarSimulator->Initialize(theEvent, fCfgFile);
	fG4TandarSimulator->RunSimulation(theEvent);
	/*************************************************
		
		Geant4 simulation ended here!
		What happens next is up to you =)

	**************************************************/
	fG4TandarSimulator->WriteEventInfo(theEvent);
	time(&end);
	
	// Calculating total time taken by the program.
	double time_taken = double(end - start);
	cout << "[INFO] G4TandarSimulator: Time taken by program is : " << fixed
			 << time_taken << setprecision(5);
	cout << " sec " << endl;

	return 0;

}

void
G4TandarSimulator::Initialize(Event& theEvent, string cfgFile)
{

	cout << "[INFO] G4TandarSimulator::Initialize" << endl;
	cout << "[INFO] G4TandarSimulator::Initialize: Reading configuration file " << cfgFile << endl;

	// Fill Event object from configuration file
	// Read Simulation configuration
	theEvent = ConfigManager::ReadConfigurationFile(cfgFile);
	// get simulation simulation settings
	const Event::Config &cfg = theEvent.GetConfig();
	ConfigManager::PrintConfig(cfg);
	// Read Detector Configuration
	ConfigManager::ReadDetectorList(cfg.fDetectorList, theEvent);
	
}            


bool
G4TandarSimulator::RunSimulation(Event& theEvent)
{

	cout << "[INFO] G4TandarSimulator::RunSimulation" << endl;

	const Event::Config &cfg = theEvent.GetConfig();
	SimData& simData = theEvent.GetSimData();
	const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();
	cout << "[INFO] G4TandarSimulator::RunSimulation: Number of particles to be simulated = " << NumberOfParticles << endl;

	
	if (!NumberOfParticles) {
		cerr << "[ERROR] G4TandarSimulator::RunSimulation: No Particles in the Event! Exiting." << endl;
		return false;
	}
	
	
	/***************

		Geant4 Setup    

	*****************/

	G4long myseed = time(NULL);
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(myseed);
	cout << "Seed for random generation: " << myseed << endl;

	G4VisManager* fVisManager = nullptr;
	
	// construct the default run manager
	auto fRunManager = G4RunManagerFactory::CreateRunManager();

	// set mandatory initialization classes
	auto fDetConstruction = new G4TandarDetectorConstruction(theEvent);
	fRunManager->SetUserInitialization(fDetConstruction);
	
	fRunManager->SetUserInitialization(new G4TandarPhysicsList(fPhysicsName));  
 
	G4MPrimaryGeneratorAction *fPrimaryGenerator = new G4MPrimaryGeneratorAction(theEvent);
	fRunManager->SetUserAction(fPrimaryGenerator);
	
	G4TandarRunAction *fRunAction = new G4TandarRunAction(theEvent);
	fRunManager->SetUserAction(fRunAction);
	
	G4TandarEventAction *fEventAction = new G4TandarEventAction();
	fRunManager->SetUserAction(fEventAction);

	fRunManager->SetUserAction(new G4TandarTrackingAction());

	G4TandarSteppingAction *fSteppingAction = new G4TandarSteppingAction(fDetConstruction, fEventAction, theEvent);
	fRunManager->SetUserAction(fSteppingAction);
	
	// initialize G4 kernel
	fRunManager->Initialize();

	// initialize visualization
	if ((cfg.fGeoVis || cfg.fTrajVis) && !fVisManager)
		fVisManager = new G4VisExecutive;

	// get the pointer to the UI manager and set verbosities
	G4UImanager* fUImanager = G4UImanager::GetUIpointer();
	switch (cfg.fVerbosity) {
		case 1:
			fUImanager->ApplyCommand("/run/verbose 1");
			fUImanager->ApplyCommand("/event/verbose 0");
			fUImanager->ApplyCommand("/tracking/verbose 0");
			break;
		case 2:
			fUImanager->ApplyCommand("/run/verbose 1");
			fUImanager->ApplyCommand("/event/verbose 1");
			fUImanager->ApplyCommand("/tracking/verbose 0");
			break;
		case 3:
			fUImanager->ApplyCommand("/run/verbose 1");
			fUImanager->ApplyCommand("/event/verbose 1");
			fUImanager->ApplyCommand("/tracking/verbose 1");
			break;
		default:
			fUImanager->ApplyCommand("/run/verbose 0");
			fUImanager->ApplyCommand("/event/verbose 0");
			fUImanager->ApplyCommand("/tracking/verbose 0");
		}
	
	if (cfg.fGeoVis || cfg.fTrajVis) {
		fVisManager->Initialize();
		fUImanager->ApplyCommand(("/vis/open " + fRenderFile).c_str());
		fUImanager->ApplyCommand("/vis/scene/create");
		fUImanager->ApplyCommand("/vis/sceneHandler/attach");
		fUImanager->ApplyCommand("/vis/scene/add/volume");
		fUImanager->ApplyCommand("/vis/scene/add/axes");
		fUImanager->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 0. 0.");
		fUImanager->ApplyCommand("/vis/viewer/set/targetPoint 0 0 0");
		fUImanager->ApplyCommand("/vis/viewer/zoom 1");
		fUImanager->ApplyCommand("/vis/viewero/set/style/wireframe");
		fUImanager->ApplyCommand("/vis/drawVolume");
		fUImanager->ApplyCommand("/vis/scene/notifyHandlers");
		fUImanager->ApplyCommand("/vis/viewer/update");
	}
	if (cfg.fTrajVis) {
		fUImanager->ApplyCommand("/tracking/storeTrajectory 1");
		fUImanager->ApplyCommand("/vis/scene/add/trajectories");
		fUImanager->ApplyCommand("/vis/filtering/trajectories/create/particleFilter");
		// for debugging purposes, gammas are not drawn
		fUImanager->ApplyCommand("/vis/filtering/trajectories/particleFilter-0/add opticalphoton");
		fUImanager->ApplyCommand("/vis/filtering/trajectories/particleFilter-0/invert true");
	}


	// loop over particle vector
	for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
		G4TandarSimulator::currentParticle = *it;
		simData.SetCurrentParticle(*it);
		// Run simulation
		fRunManager->BeamOn(1);

	}


	delete fVisManager;
	delete fRunManager;

	cout << "[INFO] G4TandarSimulator::RunSimulation: Geant4 Simulation ended successfully. " << endl;


	return true;

}


void
G4TandarSimulator::WriteEventInfo(Event& theEvent)
{

	cout << "[INFO] G4TandarSimulator::WriteEventInfo" << endl;
	#warning "FileWriter disabled to not overwrite output.dat files"
	cout << "[WARNING] G4TandarSimulator::WriteEventInfo: FileWriter disabled to not overwrite output.dat files" << endl;
	// DataWriter::FileWriter(theEvent);

	return;

}
