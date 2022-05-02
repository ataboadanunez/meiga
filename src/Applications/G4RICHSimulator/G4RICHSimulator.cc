/* 
	
	Application for simulating the muon decay in a WCD

	author: alvaro taboada
	date: 9 Feb 2022

 */

// Headers of this particular application
#include "G4RICHSimulator.h"
#include "G4RICHConstruction.h"
#include "G4RICHPrimaryGeneratorAction.h"
#include "G4RICHStackingAction.h"
#include "G4RICHEventAction.h"
#include "G4RICHRunAction.h"
#include "G4RICHTrackingAction.h"
#include "G4RICHSteppingAction.h"

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

Particle G4RICHSimulator::currentParticle;
G4RICHSimulator* fG4RICHSimulator;
string fCfgFile;

G4RICHSimulator::G4RICHSimulator()
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

	fG4RICHSimulator = new G4RICHSimulator();
	// Create Event object
	Event theEvent;
	fG4RICHSimulator->Initialize(theEvent, fCfgFile);
	fG4RICHSimulator->RunSimulation(theEvent);
	/*************************************************
		
		Geant4 simulation ended here!
		What happens next is up to you =)

	**************************************************/
	fG4RICHSimulator->WriteEventInfo(theEvent);
	time(&end);

	// Calculating total time taken by the program.
	double time_taken = double(end - start);
	cout << "[INFO] G4RICHSimulator: Time taken by program is : " << fixed
			 << time_taken << setprecision(5);
	cout << " sec " << endl;
	
	return 0;

}

void
G4RICHSimulator::Initialize(Event& theEvent, string cfgFile)
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

	cout << "[INFO] G4RICHSimulator::Initialize: Using the following configuration:" << endl;
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
G4RICHSimulator::RunSimulation(Event& theEvent)
{

	cout << "[INFO] G4RICHSimulator::RunSimulation" << endl;
	
	SimData& simData = theEvent.GetSimData();
	const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();
	cout << "[INFO] G4RICHSimulator::RunSimulation: Number of particles to be simulated = " << NumberOfParticles << endl;
	
	if (!NumberOfParticles) {
		cerr << "[ERROR] G4RICHSimulator::RunSimulation: No Particles in the Event! Exiting." << endl;
		return false;
	}
	
	simData.SetInjectionMode(fInjectionMode);
	if (fInjectionMode)
		cout << "[INFO] G4RICHSimulator::RunSimulation: Partilces injected INSIDE the detector (SimulationMode = " << fSimulationMode << "). " << endl;

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
	auto fDetConstruction = new G4RICHConstruction(theEvent);
	fRunManager->SetUserInitialization(fDetConstruction);
	
	fRunManager->SetUserInitialization(new G4MPhysicsList(fPhysicsName));

	G4RICHPrimaryGeneratorAction *fPrimaryGenerator = new G4RICHPrimaryGeneratorAction(theEvent);
	fRunManager->SetUserAction(fPrimaryGenerator);
	
	G4RICHStackingAction *fStackingAction = new G4RICHStackingAction(theEvent);
	fRunManager->SetUserAction(fStackingAction);

	G4RICHRunAction *fRunAction = new G4RICHRunAction();
	fRunManager->SetUserAction(fRunAction);
	
	G4RICHEventAction *fEventAction = new G4RICHEventAction(theEvent);
	fRunManager->SetUserAction(fEventAction);

	fRunManager->SetUserAction(new G4RICHTrackingAction());

	G4RICHSteppingAction *fSteppingAction = new G4RICHSteppingAction(fDetConstruction, fEventAction, theEvent);
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
		G4RICHSimulator::currentParticle = *it;
		simData.SetCurrentParticle(*it);
		// Run simulation
		fRunManager->BeamOn(1);
	}


	delete fVisManager;
	delete fRunManager;

	cout << "[INFO] G4RICHSimulator::RunSimulation: Geant4 Simulation ended successfully. " << endl;

	return true;

}


void
G4RICHSimulator::WriteEventInfo(Event& theEvent)
{
	cout << "[INFO] G4RICHSimulator::WriteEventInfo" << endl;

	// for accessing Simulated Data at Detector/Event level
	SimData& simData = theEvent.GetSimData();
	cout << "[INFO] G4RICHSimulator::WriteEventInfo: Accessing DetectorSimData" << endl;
	// loop over detector range
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {

		auto& currDet = detIt->second;
		int detId = currDet.GetId();

		DetectorSimData& detSimData = simData.GetDetectorSimData(detId);
		// get number of optical devices in the detector
		int nOptDev = currDet.GetNOptDevice();
		cout << "[INFO] G4RICHSimulator::WriteEventInfo: Accessing data of detector " << detId << " with " << nOptDev << " optical devices." << endl;

		// loop over optical devices
		for (auto odIt = currDet.OptDeviceRange().begin(); odIt != currDet.OptDeviceRange().end(); odIt++) {
			auto& currOd = odIt->second;
			int odId = currOd.GetId();

			OptDeviceSimData& odSimData = detSimData.GetOptDeviceSimData(odId);
			cout << "[INFO] G4RICHSimulator::WriteEventInfo: Accessing signal of " << currOd.GetName() << " " << odId << " from Detector " << detId << endl;

			// checking signal at optical devices
			const auto *peTimeDistributionRange = odSimData.PETimeDistributionRange();
			if (!peTimeDistributionRange) {
				cerr << "No Time for this channel!" << endl;
				continue;
			}

			cout << endl;
			// accessing signals for different particle components

			for (int compIt = Particle::eElectromagnetic; compIt < Particle::eEnd; compIt++) {
				
				Particle::Component particleComponent = static_cast<Particle::Component>(compIt);
				// using currentParticle to get the component name. its ugly but should do the trick
				auto componentName = G4RICHSimulator::currentParticle.GetComponentName(particleComponent);
				cout << "[INFO] G4RICHSimulator::WriteEventInfo: Accessing signals of particle component " << componentName << " (" << particleComponent << ")" <<  endl;
				try 
				{ 
					const auto peTimeDistributionRangeComp = odSimData.PETimeDistributionRange(particleComponent);
					if (!peTimeDistributionRange) {
						cerr << "[INFO] No time distribution for particle component " << componentName << endl;
						continue;
					}

					ofstream* fPEFile = new ofstream();
					string outputfileName = "photoelectrons_"+std::to_string(detId)+"_PMT_"+std::to_string(odId)+"_"+std::to_string(particleComponent)+".dat";
					fPEFile->open(outputfileName);
					for (auto peTime = peTimeDistributionRangeComp.begin(); peTime != peTimeDistributionRangeComp.end(); ++peTime) {
						size_t npe = peTime->size();
						(*fPEFile) << npe << " ";
					}
					
					(*fPEFile) << endl;
					cout << "[INFO] G4RICHSimulator::WriteEventInfo: Signals of component " << componentName << " copied to file: " << outputfileName << endl;
					fPEFile->close();
				}
				catch (std::out_of_range &e)
				{
					cerr << "[INFO] No time distribution for particle component " << componentName << "! (Exception: " << e.what() << " )." << endl;
				}
				
				
			
			}


		}


	}

}
