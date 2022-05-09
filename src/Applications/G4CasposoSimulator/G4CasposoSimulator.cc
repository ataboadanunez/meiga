/* 
	
	Application for simulating Muography experiment at
	Mina Casposo, San Juan, Argentina

	author: alvaro taboada
	date: 6 May 2022

 */

// Headers of this particular application
#include "G4CasposoSimulator.h"
#include "G4CasposoDetectorConstruction.h"
#include "G4CasposoPrimaryGeneratorAction.h"
#include "G4CasposoEventAction.h"
#include "G4CasposoRunAction.h"
#include "G4CasposoTrackingAction.h"
#include "G4CasposoSteppingAction.h"
#include "G4CasposoPhysicsList.h" 

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

using namespace std;

Particle G4CasposoSimulator::currentParticle;
G4CasposoSimulator* fG4CasposoSimulator;
string fCfgFile;

G4CasposoSimulator::G4CasposoSimulator()
{

}

namespace 
{
	void ProgramUsage() 
	{
		cerr << " Program Usage: " << endl;
		cerr << " ./G4RICHSimulator [ -c ConfigFile.json ] " << endl;
	}

}


int main(int argc, char** argv) 
{

	if (argc < 3) {
		ProgramUsage();
		throw invalid_argument("[ERROR] G4RICHSimulator::main: A configuration file is needed!");
	}

	for (int i=1; i<argc; i=i+2) {
		string sarg(argv[i]);
		if (sarg == "-c")
			fCfgFile = argv[i+1];
	}

	// for program time calculation
	time_t start, end;
	time(&start);

	fG4CasposoSimulator = new G4CasposoSimulator();
	// Create Event object
	Event theEvent;
	fG4CasposoSimulator->Initialize(theEvent, fCfgFile);
	fG4CasposoSimulator->RunSimulation(theEvent);
	/*************************************************
		
		Geant4 simulation ended here!
		What happens next is up to you =)

	**************************************************/
	fG4CasposoSimulator->WriteEventInfo(theEvent);
	time(&end);
	
	// Calculating total time taken by the program.
	double time_taken = double(end - start);
	cout << "[INFO] G4CasposoSimulator: Time taken by program is : " << fixed
			 << time_taken << setprecision(5);
	cout << " sec " << endl;

	return 0;

}

void
G4CasposoSimulator::Initialize(Event& theEvent, string cfgFile)
{

	cout << "[INFO] G4CasposoSimulator::Initialize" << endl;
	cout << "[INFO] G4CasposoSimulator::Initialize: Reading configuration file " << cfgFile << endl;
	// Fill Event object from configuration file
	// Read Simulation configuration
	theEvent = ConfigManager::ReadConfiguration(cfgFile);
	// get simulation simulation settings
	SimData& simData = theEvent.GetSimData();
	fInputFile = simData.GetInputFileName();
	fOutputFile = simData.GetOutputFileName();
	fDetectorList = simData.GetDetectorList();
	fSimulationMode = simData.GetSimulationMode();
	fInjectionMode  = simData.GetInjectionMode();
	fGeoVisOn = simData.VisualizeGeometry();
	fTrajVisOn = simData.VisualizeTrajectory();
	fPhysicsName = simData.GetPhysicsListName();

	cout << "[INFO] G4CasposoSimulator::Initialize: Using the following configuration:" << endl;
	cout << "[INFO] InputFile = " << fInputFile << endl;
	cout << "[INFO] OutputFile = " << fOutputFile << endl;
	cout << "[INGO] DetectorList = " << fDetectorList << endl;
	cout << "[INFO] SimulationMode = " << simData.GetSimulationModeName() << endl;
	cout << "[INFO] InjectionMode = " << simData.GetInjectionModeName() << endl;
	cout << "[INFO] VisualizeGeometry = " << (fGeoVisOn ? "yes" : "no") << endl;
	cout << "[INFO] VisualizeTrajectory = " << (fTrajVisOn ? "yes" : "no") << endl;
	cout << "[INFO] RenderFile = " << fRenderFile << endl;
	cout << "[INFO] PhysicsList = " << fPhysicsName << endl;


	// Read Detector Configuration
	ConfigManager::ReadDetectorList(fDetectorList, theEvent);
	
}            


bool
G4CasposoSimulator::RunSimulation(Event& theEvent)
{

	cout << "[INFO] G4CasposoSimulator::RunSimulation" << endl;

	SimData& simData = theEvent.GetSimData();
	const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();
	cout << "[INFO] G4CasposoSimulator::RunSimulation: Number of particles to be simulated = " << NumberOfParticles << endl;

	
	if (!NumberOfParticles) {
		cerr << "[ERROR] G4CasposoSimulator::RunSimulation: No Particles in the Event! Exiting." << endl;
		return false;
	}
	
	simData.SetInjectionMode(fInjectionMode);
	if (fInjectionMode == SimData::eInsideDetector)
		cout << "[INFO] G4CasposoSimulator::RunSimulation: Partilces injected INSIDE the detector (InjectionMode = " << fInjectionMode << "). " << endl;

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
	auto fDetConstruction = new G4CasposoDetectorConstruction(theEvent);
	fRunManager->SetUserInitialization(fDetConstruction);
	
	fRunManager->SetUserInitialization(new G4CasposoPhysicsList(fPhysicsName));  
 
	G4CasposoPrimaryGeneratorAction *fPrimaryGenerator = new G4CasposoPrimaryGeneratorAction(theEvent);
	fRunManager->SetUserAction(fPrimaryGenerator);
	
	G4CasposoRunAction *fRunAction = new G4CasposoRunAction();
	fRunManager->SetUserAction(fRunAction);
	
	G4CasposoEventAction *fEventAction = new G4CasposoEventAction();
	fRunManager->SetUserAction(fEventAction);

	fRunManager->SetUserAction(new G4CasposoTrackingAction());

	G4CasposoSteppingAction *fSteppingAction = new G4CasposoSteppingAction(fDetConstruction, fEventAction, theEvent);
	fRunManager->SetUserAction(fSteppingAction);
	
	// initialize G4 kernel
	fRunManager->Initialize();

	// initialize visualization
	if ((fGeoVisOn || fTrajVisOn) && !fVisManager)
		fVisManager = new G4VisExecutive;

	// get the pointer to the UI manager and set verbosities
	G4UImanager* fUImanager = G4UImanager::GetUIpointer();
	switch (fVerbosity) {
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
	
	if (fGeoVisOn || fTrajVisOn) {
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
	if (fTrajVisOn) {
		fUImanager->ApplyCommand("/tracking/storeTrajectory 1");
		fUImanager->ApplyCommand("/vis/scene/add/trajectories");
	}


	// loop over particle vector
	for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
		G4CasposoSimulator::currentParticle = *it;
		// Run simulation
		fRunManager->BeamOn(1);

	}


	delete fVisManager;
	delete fRunManager;

	cout << "[INFO] G4CasposoSimulator::RunSimulation: Geant4 Simulation ended successfully. " << endl;


	return true;

}


void
G4CasposoSimulator::WriteEventInfo(Event& theEvent)
{
	(void) theEvent;
	cout << "[INFO] G4CasposoSimulator::WriteEventInfo" << endl;

	cout << "Still nothing to write :(" << endl;


}
