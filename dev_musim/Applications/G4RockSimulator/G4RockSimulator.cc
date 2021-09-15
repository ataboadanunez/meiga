/* main script of a simple application to simulate charged
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

#include "CentralConfig.h"
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
#include <boost/foreach.hpp>

using namespace boost::property_tree;
using namespace std;

Particle G4RockSimulator::currentParticle;
//G4RockSimulator::fOutputFile->open("SiPMTraces2.dat");
G4RockSimulator* fG4RockSimulator;

G4RockSimulator::G4RockSimulator() 
{
  
}


G4RockSimulator::~G4RockSimulator()
{
  
}

// WRITE THIS FUNCTION
void
G4RockSimulator::ReadConfiguration(string filename) 
{
	cout << "G4RockSimulator::ReadConfiguration: Reading configuration from file " << filename << endl;

	ptree tree;
	
	read_xml(filename, tree, xml_parser::no_concat_text | xml_parser::no_comments);
	
	for (const auto& i : tree.get_child("G4RockSimulator")) {
		cout << "[DEBUG] G4RockSimulator::ReadConfiguration: Checking branch name " << i.first << endl;

		write_info(cout, i.second);
		if (i.first == "<xmlattr>") {
			continue;
		}


		if (i.first == "InputFile") {
			fInputFile = i.second.get_value<string>();
			cout << "InputFile is \"" << fInputFile << "\"" << endl;
		}

		//for (const auto &v : i.second.get_child("")) {
		//	string label = v.first;
		//	cout << "sub-branch name " << label << endl;
		//}
		 
	}

	exit(EXIT_SUCCESS);
}

int main() 
{
  
	/*******************

	 	Application SetUp

	********************/ 
	// MOVE TO CONFIG FILE
	string configFile = "./G4RockSimulator.xml";
	int fVerbosity = 1;
	bool fGeoVisOn = true;
	bool fTrajVisOn = true;
	G4long myseed = time(NULL);
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(myseed);
	G4cout << "Seed for random generation: " << myseed << G4endl;
	string physicsName = "QGSP_BERT_HP";
	string fRenderFile = "VRML2FILE";

	/******************
			
		Input / Output

	******************/

	// MOVE TO CONFIG FILE
	string fInputFile = "/home/alvaro/data/sims/vertical_muon.txt";
	ofstream* fOutputFile = new ofstream();
	fOutputFile->open("SiPMTraces_reflections.dat");
	ofstream* fParticleInfo = new ofstream();
	fParticleInfo->open("particle_information.dat");


	/****************
			
		Event Creation

	******************/

	Event theEvent = ReadParticleFile::EventFileReader(fInputFile);

	SimData& simData = theEvent.GetSimData();
	const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();

	cout << "[INFO] Event::SimData: Total number of particles in file = " << NumberOfParticles << endl;

	if (!NumberOfParticles) {
		cerr << "ERROR! No Particles in the Event! Exiting..." << endl;
	  return -1;
	}

  
	/***************

		Geant4 Setup		

	*****************/

	G4VisManager* fVisManager = nullptr;

	// construct the default run manager
	auto fRunManager = G4RunManagerFactory::CreateRunManager();

	// set mandatory initialization classes
	auto fDetConstruction = new G4RockDetectorConstruction(theEvent);
	fRunManager->SetUserInitialization(fDetConstruction);

	fRunManager->SetUserInitialization(new G4RockPhysicsList(physicsName));

	G4RockPrimaryGeneratorAction *fPrimaryGenerator = new G4RockPrimaryGeneratorAction();
	fRunManager->SetUserAction(fPrimaryGenerator);

	G4RockRunAction *fRunAction = new G4RockRunAction();
	fRunManager->SetUserAction(fRunAction);

	G4RockEventAction *fEventAction = new G4RockEventAction(fRunAction, fPrimaryGenerator);
	fRunManager->SetUserAction(fEventAction);

	fRunManager->SetUserAction(new G4RockTrackingAction());

	G4RockSteppingAction *fSteppingAction = new G4RockSteppingAction(fDetConstruction, fEventAction, fOutputFile, theEvent);
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
		// checks
		const int pId = it->GetParticleId();
		const double pMass = it->GetMass();
		const double pMomentum = it->GetMomentum();
		const double pKineticE = it->GetKineticEnergy();

		(*fParticleInfo) << pId << " " << pMass / MeV << " " << pMomentum / GeV << " " << pKineticE / GeV << endl;
		
		// Run simulation
		
		fRunManager->BeamOn(1);
	}

	/*************************************************

		eant4 simulation ended here!
		What happens next are calculations and output writing 

	**************************************************/

	// print simulation output. write external function...
	// Compute Detector Signal
  
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


	delete fVisManager;
	delete fRunManager;
	fOutputFile->close();
	fParticleInfo->close();
	return 0;

}

