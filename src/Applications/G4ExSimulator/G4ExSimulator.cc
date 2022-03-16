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
#include "ReadParticleFile.h"
#include "Event.h"
#include "SimData.h"
#include "OptDeviceSimData.h"
#include "Detector.h"
#include "OptDevice.h"
#include "G4MPhysicsList.h"

// boost libraries
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace boost::property_tree;
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
		cerr << " ./exeMeigaApp [ -c ConfigFile.json ] " << endl;
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


	Event theEvent;
	fG4ExSimulator = new G4ExSimulator();
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
    cout << "Time taken by program is : " << fixed
         << time_taken << setprecision(5);
    cout << " sec " << endl;

	return 0;

}

void
G4ExSimulator::Initialize(Event& theEvent, string fileName)
{

	cout << "[INFO] G4ExSimulator::Initialize" << endl;
	cout << "[INFO] G4ExSimulator::Initialize: Reading configuration file " << fileName << endl;
	// set value of flags according to cfg file
	// reading cfg file using boost
	// eventually as input of executable main(char** fConfig)
	
	fInputFile.clear();
	fOutputFile.clear();

	ptree root;
	read_json(fileName, root);

	fInputFile = root.get<string>("InputFile");
	fOutputFile = root.get<string>("OutputFile");
	// for this example application simulation mode is kept to FULL by default
	fSimulationMode = root.get<string>("SimulationMode");
	fDetectorList = root.get<string>("DetectorList");
	fGeoVisOn = root.get<bool>("GeoVisOn");
	fTrajVisOn = root.get<bool>("TrajVisOn");
	fVerbosity = root.get<int>("Verbosity");
	fRenderFile = root.get<string>("RenderFile");
	fPhysicsName = root.get<string>("PhysicsName");
	
	// Creates event from file reader
	theEvent = ReadParticleFile::EventFileReader(fInputFile);

	// Read Detector Configuration
	ConfigManager::ReadDetectorList(fDetectorList, theEvent);
	
}            


bool
G4ExSimulator::RunSimulation(Event& theEvent)
{

	cout << "[INFO] G4ExSimulator::RunSimulation" << endl;
	SimData& simData = theEvent.GetSimData();
	simData.SetSimulationMode(fSimulationMode);
  cout << "[INFO] G4ExSimulator::RunSimulation: Simulation mode selected = " << fSimulationMode << endl;
	const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();
	cout << "[INFO] G4ExSimulator::RunSimulation: Total number of particles in file = " << NumberOfParticles << endl;
	if (!NumberOfParticles) {
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
	
	//G4VModularPhysicsList* physicsList = new FTFP_BERT;
	//fRunManager->SetUserInitialization(physicsList);
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
