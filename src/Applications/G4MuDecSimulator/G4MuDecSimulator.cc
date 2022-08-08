/* 
	
	Application for simulating the muon decay in a WCD

	author: alvaro taboada
	date: 9 Feb 2022

 */

// Headers of this particular application
#include "G4MuDecSimulator.h"
#include "G4MuDecConstruction.h"
#include "G4MuDecPrimaryGeneratorAction.h"
#include "G4MuDecStackingAction.h"
#include "G4MuDecEventAction.h"
#include "G4MuDecRunAction.h"
#include "G4MuDecTrackingAction.h"
#include "G4MuDecSteppingAction.h"

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

Particle G4MuDecSimulator::currentParticle;
G4MuDecSimulator* fG4MuDecSimulator;
string fCfgFile;

G4MuDecSimulator::G4MuDecSimulator()
{

}

namespace 
{
	void ProgramUsage() 
	{
		cerr << " Program Usage: " << endl;
		cerr << " ./G4MuDecSimulator [ -c ConfigFile.json ] " << endl;
	}

}


int main(int argc, char** argv) 
{

	if (argc < 3) {
		ProgramUsage();
		throw invalid_argument("[ERROR] G4MuDecSimulator::main: A configuration file is needed!");
	}

	for (int i=1; i<argc; i=i+2) {
		string sarg(argv[i]);
		if (sarg == "-c")
			fCfgFile = argv[i+1];
	}

	// for program time calculation
	time_t start, end;
	time(&start);

	fG4MuDecSimulator = new G4MuDecSimulator();
	// Create Event object
	Event theEvent;
	fG4MuDecSimulator->Initialize(theEvent, fCfgFile);
	fG4MuDecSimulator->RunSimulation(theEvent);
	/*************************************************
		
		Geant4 simulation ended here!
		What happens next is up to you =)

	**************************************************/
	fG4MuDecSimulator->WriteEventInfo(theEvent);
	time(&end);

	// Calculating total time taken by the program.
	double time_taken = double(end - start);
	cout << "[INFO] G4MuDecSimulator: Time taken by program is : " << fixed
			 << time_taken << setprecision(5);
	cout << " sec " << endl;
	
	return 0;

}

void
G4MuDecSimulator::Initialize(Event& theEvent, string cfgFile)
{

	cout << "[INFO] G4MuDecSimulator::Initialize" << endl;
	cout << "[INFO] Reading configuration file " << cfgFile << endl;

	// Fill Event object from configuration file
	// Read Simulation configuration
	theEvent = ConfigManager::ReadConfigurationFile(cfgFile);
	// get simulation simulation settings
	const Event::Config &cfg = theEvent.GetConfig();
	// Read Detector Configuration
	ConfigManager::ReadDetectorList(cfg.fDetectorList, theEvent);

}            


bool
G4MuDecSimulator::RunSimulation(Event& theEvent)
{

	cout << "[INFO] G4MuDecSimulator::RunSimulation" << endl;
	
	const Event::Config &cfg = theEvent.GetConfig();
	SimData& simData = theEvent.GetSimData();
	const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();
	cout << "[INFO] G4MuDecSimulator::RunSimulation: Number of particles to be simulated = " << NumberOfParticles << endl;
	
	if (!NumberOfParticles) {
		cerr << "[ERROR] G4MuDecSimulator::RunSimulation: No Particles in the Event! Exiting." << endl;
		return false;
	}



	/***************

		Geant4 Setup    

	*****************/

	G4long myseed = time(NULL);
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(myseed);

	G4VisManager* fVisManager = nullptr;
	
	// construct the default run manager
	auto fRunManager = G4RunManagerFactory::CreateRunManager();

	// set mandatory initialization classes
	auto fDetConstruction = new G4MuDecConstruction(theEvent);
	fRunManager->SetUserInitialization(fDetConstruction);
	
	fRunManager->SetUserInitialization(new G4MPhysicsList(cfg.fPhysicsListName));

	G4MuDecPrimaryGeneratorAction *fPrimaryGenerator = new G4MuDecPrimaryGeneratorAction(theEvent);
	fRunManager->SetUserAction(fPrimaryGenerator);
	
	G4MuDecStackingAction *fStackingAction = new G4MuDecStackingAction(theEvent);
	fRunManager->SetUserAction(fStackingAction);

	G4MuDecRunAction *fRunAction = new G4MuDecRunAction();
	fRunManager->SetUserAction(fRunAction);
	
	G4MuDecEventAction *fEventAction = new G4MuDecEventAction(theEvent);
	fRunManager->SetUserAction(fEventAction);

	fRunManager->SetUserAction(new G4MuDecTrackingAction(fEventAction, theEvent));

	G4MuDecSteppingAction *fSteppingAction = new G4MuDecSteppingAction(fDetConstruction, fEventAction, theEvent);
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
		fUImanager->ApplyCommand(("/vis/open " + cfg.fRenderFile).c_str());
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
		G4MuDecSimulator::currentParticle = *it;
		simData.SetCurrentParticle(*it);
		// Run simulation
		fRunManager->BeamOn(1);
	}


	delete fVisManager;
	delete fRunManager;

	cout << "[INFO] G4MuDecSimulator::RunSimulation: Geant4 Simulation ended successfully. " << endl;

	return true;

}


void
G4MuDecSimulator::WriteEventInfo(Event& theEvent)
{
	cout << "[INFO] G4MuDecSimulator::WriteEventInfo" << endl;

	DataWriter::FileWriter(theEvent);
	
	return;

}
