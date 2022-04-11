/* 
	
	Main script of an example application using Geant4

	author: alvaro taboada
	date: 15 Sep 2021

	$Id:$

 */

// Headers of this particular application
#include "G4ExSimulator.h"
#include "G4ExDetectorConstruction.h"
#include "G4ExPrimaryGeneratorAction.h"
#include "G4ExEventAction.h"
#include "G4ExRunAction.h"
#include "G4ExTrackingAction.h"
#include "G4ExSteppingAction.h"

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

using namespace std;

Particle G4ExSimulator::currentParticle;
G4ExSimulator* fG4ExSimulator;
string fCfgFile;

G4ExSimulator::G4ExSimulator()
{

}

namespace 
{
	void ProgramUsage() 
	{
		cerr << " Program Usage: " << endl;
		cerr << " ./G4ExSimulator [ -c ConfigFile.json ] " << endl;
	}

}

int main(int argc, char** argv) 
{

	if (argc < 3) {
		ProgramUsage();
		throw invalid_argument("[ERROR] G4ExSimulator::main: A configuration file is needed!");
	}

	for (int i=1; i<argc; i=i+2) {
		string sarg(argv[i]);
		if (sarg == "-c")
			fCfgFile = argv[i+1];
	}
	
	// for program time calculation
  time_t start, end;
  time(&start);
  
	fG4ExSimulator = new G4ExSimulator();
	// Create Event object
	Event theEvent;
	fG4ExSimulator->Initialize(theEvent, fCfgFile);
	fG4ExSimulator->RunSimulation(theEvent);
	/*************************************************
		
		Geant4 simulation ended here!
		What happens next is up to you =)

	**************************************************/
	fG4ExSimulator->WriteEventInfo(theEvent);
	time(&end);

	// Calculating total time taken by the program.
    double time_taken = double(end - start);
    cout << "[INFO] G4ExSimulator: Time taken by program is : " << fixed
         << time_taken << setprecision(5);
    cout << " sec " << endl;

	return 0;

}

void
G4ExSimulator::Initialize(Event& theEvent, string cfgFile)
{

	cout << "[INFO] G4ExSimulator::Initialize" << endl;
	cout << "[INFO] G4ExSimulator::Initialize: Reading configuration file " << cfgFile << endl;

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

	cout << "[INFO] G4ExSimulator::Initialize: Using the following configuration:" << endl;
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
G4ExSimulator::RunSimulation(Event& theEvent)
{

	cout << "[INFO] G4ExSimulator::RunSimulation" << endl;
	
	SimData& simData = theEvent.GetSimData();
	const unsigned int numberOfParticles = simData.GetTotalNumberOfParticles();
	cout << "[INFO] G4ExSimulator::RunSimulation: Number of particles to be simulated = " << numberOfParticles << endl;
	if (!numberOfParticles) {
		cerr << "[ERROR] G4ExSimulator::RunSimulation: No Particles in the Event! Exiting..." << endl;
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
	auto fDetConstruction = new G4ExDetectorConstruction(theEvent);
	fRunManager->SetUserInitialization(fDetConstruction);
	
	fRunManager->SetUserInitialization(new G4MPhysicsList(fPhysicsName));

	G4ExPrimaryGeneratorAction *fPrimaryGenerator = new G4ExPrimaryGeneratorAction();
	fRunManager->SetUserAction(fPrimaryGenerator);
	
	G4ExRunAction *fRunAction = new G4ExRunAction();
	fRunManager->SetUserAction(fRunAction);
	
	G4ExEventAction *fEventAction = new G4ExEventAction();
	fRunManager->SetUserAction(fEventAction);

	fRunManager->SetUserAction(new G4ExTrackingAction());

	G4ExSteppingAction *fSteppingAction = new G4ExSteppingAction(fDetConstruction, fEventAction, theEvent);
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
		G4ExSimulator::currentParticle = *it;
		// Run simulation
		fRunManager->BeamOn(1);
	}


	delete fVisManager;
	delete fRunManager;

	cout << "[INFO] G4ExSimulator::RunSimulation: Geant4 Simulation ended successfully. " << endl;

	return true;

}


void
G4ExSimulator::WriteEventInfo(Event& theEvent)
{
	cout << "[INFO] G4ExSimulator::WriteEventInfo" << endl;

	// for accessing Simulated Data at Detector/Event level
	SimData& simData = theEvent.GetSimData();
	
	// loop over detector range
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {

		auto& currDet = detIt->second;
		int detId = currDet.GetId();

		DetectorSimData& detSimData = simData.GetDetectorSimData(detId);
		// get number of optical devices in the detector
		int nOptDev = currDet.GetNOptDevice();
		cout << "[INFO] G4ExSimulator::WriteEventInfo: Accessing data of detector " << detId << " with " << nOptDev << " optical devices." << endl;

		// loop over optical devices
		for (auto odIt = currDet.OptDeviceRange().begin(); odIt != currDet.OptDeviceRange().end(); odIt++) {
			auto& currOd = odIt->second;
			int odId = currOd.GetId();

			OptDeviceSimData& odSimData = detSimData.GetOptDeviceSimData(odId);
			cout << "[INFO] G4ExSimulator::WriteEventInfo: Accessing signal of " << currOd.GetName() << " " << odId << " from Detector " << detId << endl;

			// checking signal at optical devices
			const auto *peTimeDistributionRange = odSimData.PETimeDistributionRange();
			if (!peTimeDistributionRange) {
				cerr << "No Time for this channel!" << endl;
				continue;
			}

			cout << "[INFO] G4ExSimulator::WriteEventInfo: Number of photo-electrons = ";
			for (auto peTime = peTimeDistributionRange->begin(); peTime != peTimeDistributionRange->end(); ++peTime) {

				size_t npe = (*peTime)->size();
				cout << npe << " ";
				// write pulses to output file
			}

			cout << endl;


		}


	}


}
