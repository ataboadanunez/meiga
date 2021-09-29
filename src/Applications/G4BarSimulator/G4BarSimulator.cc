/* 
  
  main script of a simple application to simulate charged
  particles traversing a scintillator bar

   author: alvaro taboada
   date: 3 August 2021

   $Id:$
 */

#include "G4BarSimulator.h"
#include "G4BarDetectorConstruction.h"
//#include "G4BarPhysicsList.h"
#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"

#include "G4BarPrimaryGeneratorAction.h"
#include "G4BarEventAction.h"
#include "G4BarRunAction.h"
#include "G4BarTrackingAction.h"
#include "G4BarSteppingAction.h"

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
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace boost::property_tree;
using namespace std;

Particle G4BarSimulator::currentParticle;
G4BarSimulator* fG4BarSimulator;
std::string fCfgFile;

G4BarSimulator::G4BarSimulator()
{

}


//G4BarSimulator::~G4BarSimulator()
//{
//  
//}

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
  
  //vector<string> all_args;

  if (argc < 3) {
  	ProgramUsage();
  	throw invalid_argument("Config file needed! (See Program Usage)");
  }

  for (int i=1; i<argc; i=i+2) {
  	string sarg(argv[i]);
  	if (sarg == "-c")
  		fCfgFile = argv[i+1];
  }
  cout << "Argc = " << argc << endl;

  Event theEvent;
  fG4BarSimulator = new G4BarSimulator();
  fG4BarSimulator->Initialize(theEvent, fCfgFile);
  fG4BarSimulator->RunSimulation(theEvent);
  /*************************************************
    
    Geant4 simulation ended here!
    What happens next is up to you =)

  **************************************************/

  
  
  return 0;

}

void
G4BarSimulator::Initialize(Event& theEvent, string fileName)
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

  //fInputFile = std::string("/home/alvaro/data/sims/vertical_muon.txt");
  // Creates event from file reader
  theEvent = ReadParticleFile::EventFileReader(fInputFile);
  // this will go in WriteEventInfo()
  //fOutputFile = std::string("/home/alvaro/data/sims/output/output.dat");
  fOutput = new ofstream();
  fOutput->open(fOutputFile);
}            


bool
G4BarSimulator::RunSimulation(Event& theEvent)
{

  SimData& simData = theEvent.GetSimData();
  const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();

  std::cout << "[INFO] Event::SimData: Total number of particles in file = " << NumberOfParticles << std::endl;
  
  if (!NumberOfParticles) {
    std::cerr << "ERROR! No Particles in the Event! Exiting..." << std::endl;
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
  auto fDetConstruction = new G4BarDetectorConstruction(theEvent);
  fRunManager->SetUserInitialization(fDetConstruction);
  
  //fRunManager->SetUserInitialization(new G4BarPhysicsList(physicsName));
  // Physics list for optical processes
  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  physicsList->RegisterPhysics(opticalPhysics);
  fRunManager->SetUserInitialization(physicsList);
  
  G4BarPrimaryGeneratorAction *fPrimaryGenerator = new G4BarPrimaryGeneratorAction();
  fRunManager->SetUserAction(fPrimaryGenerator);
  
  G4BarRunAction *fRunAction = new G4BarRunAction();
  fRunManager->SetUserAction(fRunAction);
  
  G4BarEventAction *fEventAction = new G4BarEventAction(fOutput, fRunAction, fPrimaryGenerator);
  fRunManager->SetUserAction(fEventAction);

  fRunManager->SetUserAction(new G4BarTrackingAction());

  G4BarSteppingAction *fSteppingAction = new G4BarSteppingAction(fDetConstruction, fEventAction, theEvent);
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

  int nParticle = 0;
  // loop over particle vector
  for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
    G4BarSimulator::currentParticle = *it;
    // Run simulation
    fRunManager->BeamOn(1);
    nParticle+=1;

    cout << "Simulated " << nParticle << " particle(s) out of " << NumberOfParticles << endl;
  }


  delete fVisManager;
  delete fRunManager;

  return true;

}
