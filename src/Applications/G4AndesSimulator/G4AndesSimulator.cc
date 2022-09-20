/* 
	
	Application for simulating the muon flux at the
	ANDES Underground Laboratory

	author: alvaro taboada
	date: 12 September 2022

 */

// Headers of this particular application
#include "G4AndesSimulator.h"
#include "G4AndesDetectorConstruction.h"
#include "G4AndesPrimaryGeneratorAction.h"
#include "G4AndesEventAction.h"
#include "G4AndesRunAction.h"
#include "G4AndesTrackingAction.h"
#include "G4AndesSteppingAction.h"
#include "G4AndesPhysicsList.h" 

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
#include "ReadParticleFile.h"
#include "Event.h"
#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"
#include "OptDevice.h"
#include "G4MPhysicsList.h"
#include "DataWriter.h"

using namespace std;

Particle G4AndesSimulator::currentParticle;
G4AndesSimulator* fG4AndesSimulator;
string fCfgFile;

G4AndesSimulator::G4AndesSimulator()
{

}

namespace 
{
	void ProgramUsage() 
	{
		cerr << " Program Usage: " << endl;
		cerr << " ./G4AndesSimulator [ -c ConfigFile.json ] " << endl;
	}

}


int main(int argc, char** argv) 
{

	if (argc < 3) {
		ProgramUsage();
		throw invalid_argument("[ERROR] G4AndesSimulator::main: A configuration file is needed!");
	}

	for (int i=1; i<argc; i=i+2) {
		string sarg(argv[i]);
		if (sarg == "-c")
			fCfgFile = argv[i+1];
	}

	// for program time calculation
	time_t start, end;
	time(&start);

	fG4AndesSimulator = new G4AndesSimulator();
	// Create Event object
	Event theEvent;
	fG4AndesSimulator->Initialize(theEvent, fCfgFile);
	fG4AndesSimulator->RunSimulation(theEvent);
	/*************************************************
		
		Geant4 simulation ended here!
		What happens next is up to you =)

	**************************************************/
	fG4AndesSimulator->WriteEventInfo(theEvent);
	time(&end);
	
	// Calculating total time taken by the program.
	double time_taken = double(end - start);
	cout << "[INFO] G4AndesSimulator: Time taken by program is : " << fixed
			 << time_taken << setprecision(5);
	cout << " sec " << endl;

	return 0;

}

void
G4AndesSimulator::Initialize(Event& theEvent, string cfgFile)
{

	cout << "[INFO] G4AndesSimulator::Initialize" << endl;
	cout << "[INFO] G4AndesSimulator::Initialize: Reading configuration file " << cfgFile << endl;

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
G4AndesSimulator::RunSimulation(Event& theEvent)
{

	cout << "[INFO] G4AndesSimulator::RunSimulation" << endl;

	const Event::Config &cfg = theEvent.GetConfig();
	SimData& simData = theEvent.GetSimData();
	const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();
	cout << "[INFO] G4AndesSimulator::RunSimulation: Number of particles to be simulated = " << NumberOfParticles << endl;

	
	if (!NumberOfParticles) {
		cerr << "[ERROR] G4AndesSimulator::RunSimulation: No Particles in the Event! Exiting." << endl;
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
	auto fDetConstruction = new G4AndesDetectorConstruction(theEvent);
	fRunManager->SetUserInitialization(fDetConstruction);
	
	fRunManager->SetUserInitialization(new G4AndesPhysicsList(fPhysicsName));  
 
	G4AndesPrimaryGeneratorAction *fPrimaryGenerator = new G4AndesPrimaryGeneratorAction(theEvent);
	fRunManager->SetUserAction(fPrimaryGenerator);
	
	G4AndesRunAction *fRunAction = new G4AndesRunAction(theEvent);
	fRunManager->SetUserAction(fRunAction);
	
	G4AndesEventAction *fEventAction = new G4AndesEventAction();
	fRunManager->SetUserAction(fEventAction);

	fRunManager->SetUserAction(new G4AndesTrackingAction());

	G4AndesSteppingAction *fSteppingAction = new G4AndesSteppingAction(fDetConstruction, fEventAction, theEvent);
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
		fUImanager->ApplyCommand("/vis/scene/add/axes 0 0 0 1 m");
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
	}


	// loop over particle vector
	for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
		G4AndesSimulator::currentParticle = *it;
		// Run simulation
		fRunManager->BeamOn(1);

	}


	delete fVisManager;
	delete fRunManager;

	cout << "[INFO] G4AndesSimulator::RunSimulation: Geant4 Simulation ended successfully. " << endl;


	return true;

}


void
G4AndesSimulator::WriteEventInfo(Event& theEvent)
{

	cout << "[INFO] G4AndesSimulator::WriteEventInfo" << endl;
	
	DataWriter::FileWriter(theEvent);

	return;

}
