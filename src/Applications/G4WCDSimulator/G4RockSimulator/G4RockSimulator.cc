/* 
		main script of a simple application to simulate charged
   	particles traversing a block of rock

   	author: alvaro taboada
   	date: 17.05.21

   	$Id:$

 */

#include "G4RockSimulator.h"
#include "G4RockDetectorConstruction.h"
#include "G4RockPhysicsList.h"
#include "G4RockPrimaryGeneratorAction.h"
#include "G4RockEventAction.h"
#include "G4RockRunAction.h"
#include "G4RockTrackingAction.h"
#include "G4RockSteppingAction.h"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"

#include "ConfigManager.h"
#include "CorsikaUtilities.h"
#include "ReadParticleFile.h"

#include "Event.h"
#include "SimData.h"
#include "SiPMSimData.h"

#include "Detector.h"
#include "Module.h"
#include "SiPM.h"

#include <string>
#include <iostream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/foreach.hpp>

using namespace boost::property_tree;
using namespace std;

Particle G4RockSimulator::currentParticle;
G4RockSimulator* fG4RockSimulator;
string fCfgFile;

G4RockSimulator::G4RockSimulator() 
{
  
}

namespace 
{
	void ProgramUsage() 
	{
		cerr << " Program Usage: " << endl;
		cerr << " ./Executable [ -c ConfigFile.json ] " << endl;
	}

}

int main(int argc, char** argv) 
{
  
  if (argc < 3) {
  	ProgramUsage();
  	throw invalid_argument("Config file needed! (See Program Usage)");
  }

  for (int i=1; i<argc; i=i+2) {
  	string sarg(argv[i]);
  	if (sarg == "-c")
  		fCfgFile = argv[i+1];
  }
  

  Event theEvent;
  fG4RockSimulator = new G4RockSimulator();
  fG4RockSimulator->Initialize(theEvent, fCfgFile);
  fG4RockSimulator->RunSimulation(theEvent);
  /*************************************************
    
    Geant4 simulation ended here!
    What happens next is up to you =)

  **************************************************/
  fG4RockSimulator->WriteEventInfo(theEvent);
  

  
  return 0;

}


void
G4RockSimulator::Initialize(Event& theEvent, string fileName)
{

	cout << "... Initialize ..." << endl;
	// set value of flags according to cfg file
	// reading cfg file using boost
	// eventually as input of executable main(char** fConfig)

	fInputFile.clear();
	fOutputFile.clear();

	ptree root;
	read_json(fileName, root);

	fInputFile = root.get<string>("InputFile");
	fOutputFile = root.get<string>("OutputFile");
	fGeoVisOn = root.get<bool>("GeoVisOn");
	fTrajVisOn = root.get<bool>("TrajVisOn");
	fVerbosity = root.get<int>("Verbosity");
	fRenderFile = root.get<string>("RenderFile");
	fPhysicsName = root.get<string>("PhysicsName");
	// Creates event from file reader
	theEvent = ReadParticleFile::EventFileReader(fInputFile);
  
}

bool
G4RockSimulator::RunSimulation(Event& theEvent)
{
	
	SimData& simData = theEvent.GetSimData();
	const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();

	cout << "[INFO] Event::SimData: Total number of particles in file = " << NumberOfParticles << endl;

	if (!NumberOfParticles) {
		cerr << "ERROR! No Particles in the Event! Exiting..." << endl;
		return false;
	}

	/***************

	Geant4 Setup    

	*****************/

	G4long myseed = time(NULL);
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(myseed);
	G4cout << "Seed for random generation: " << myseed << G4endl;

	G4VisManager* fVisManager = nullptr;

	// construct the default run manager
	auto fRunManager = G4RunManagerFactory::CreateRunManager();

	// set mandatory initialization classes
	auto fDetConstruction = new G4RockDetectorConstruction(theEvent);
	fRunManager->SetUserInitialization(fDetConstruction);

	fRunManager->SetUserInitialization(new G4RockPhysicsList(fPhysicsName));

	G4RockPrimaryGeneratorAction *fPrimaryGenerator = new G4RockPrimaryGeneratorAction();
	fRunManager->SetUserAction(fPrimaryGenerator);

	G4RockRunAction *fRunAction = new G4RockRunAction();
	fRunManager->SetUserAction(fRunAction);

	G4RockEventAction *fEventAction = new G4RockEventAction(fRunAction, fPrimaryGenerator);
	fRunManager->SetUserAction(fEventAction);

	fRunManager->SetUserAction(new G4RockTrackingAction());

	G4RockSteppingAction *fSteppingAction = new G4RockSteppingAction(fDetConstruction, fEventAction, theEvent);
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
		G4RockSimulator::currentParticle = *it;
		// Run simulation
		fRunManager->BeamOn(1);
	}

	delete fVisManager;
	delete fRunManager;

	return true;

}

void
G4RockSimulator::WriteEventInfo(Event& theEvent)
{

	SimData& simData = theEvent.GetSimData();
	Detector& detector = theEvent.GetDetector();
	int nModules = detector.GetNModules();
	cout << "Number of simulated modules = " << nModules << endl;

	for (auto modIt = detector.ModulesRange().begin(); modIt != detector.ModulesRange().end(); modIt++) {

		auto& currentMod = modIt->second;
		unsigned int moduleId = currentMod.GetId();
		//GetNBars returns number of bars per panel
		unsigned int nSipms = currentMod.GetNBars() * 2;
		DetectorSimData& detSimData = simData.GetDetectorSimData(moduleId);

		cout << "G4RockSimulator: Accessing data of Module " << moduleId << " with " << nSipms << " SiPMs" << endl;

	// iterate over SiPMs
	for (auto sipmIt = currentMod.SiPMsRange().begin(); sipmIt != currentMod.SiPMsRange().end(); sipmIt++) {

		auto& currentSiPM = sipmIt->second;
		unsigned int sipmId = currentSiPM.GetId();
		SiPMSimData& sipmSim = detSimData.GetSiPMSimData(sipmId);

		//std::vector<std::vector<double>*>* peTimeDistriution = sipmSim.PETimeDistributionRange();
		ofstream* fPETimeDistriution = new ofstream();
		ofstream* fPulses = new ofstream();

		fPETimeDistriution->open("peTimes_module_" + std::to_string(moduleId) + "_sipm_" + std::to_string(sipmId) + ".dat");
		fPulses->open("pulses_module_"+std::to_string(moduleId)+"_sipm_"+std::to_string(sipmId)+".dat");
		
		// iterate over PE time distriutions (time pulses)
		for (auto peTime = sipmSim.PETimeDistributionRange()->begin(); peTime != sipmSim.PETimeDistributionRange()->end(); ++peTime) {
			
			size_t npe = (*peTime)->size();
			// if not PE were produced, continue
			if (!npe) {
				continue;
			}
			
			auto sipmPulse = sipmSim.CalculatePulse(1*ns, *(*peTime));
			size_t pulseSize = sipmPulse.size();

			// pulse iterator
			for (size_t pulseIt=0; pulseIt<pulseSize; pulseIt++) {
				//cout << sipmPulse[pulseIt] << " ";
				(*fPulses) << sipmPulse[pulseIt] << " ";
			}

			(*fPulses) << endl;

			// PE iterator
			for (size_t peIt=0; peIt<npe; peIt++) {
				(*fPETimeDistriution) << (*peTime)->at(peIt) << " ";
			}

			(*fPETimeDistriution) << endl;

		}

		cout << "Photo-electron timedistribution of SiPM " << sipmId << " coppied!" << endl;
		fPETimeDistriution->close();
		
		cout << "Pulses of SiPM " << sipmId << " coppied!" << endl;
		fPulses->close();
		
		}
	}

}