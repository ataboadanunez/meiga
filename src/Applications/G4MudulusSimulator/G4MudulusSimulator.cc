/* 
	
	Main script of an example application using Geant4

	author: alvaro taboada
	date: 15 Sep 2021

	$Id:$

 */

// Headers of this particular application
#include "G4MudulusSimulator.h"
#include "G4MudulusDetectorConstruction.h"
#include "G4MudulusPrimaryGeneratorAction.h"
#include "G4MudulusEventAction.h"
#include "G4MudulusRunAction.h"
#include "G4MudulusTrackingAction.h"
#include "G4MudulusSteppingAction.h"

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
#include "SiPMSimData.h"
#include "Detector.h"
#include "Module.h"
#include "SiPM.h"
#include "G4MPhysicsList.h"

// boost libraries
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace boost::property_tree;
using namespace std;

Particle G4MudulusSimulator::currentParticle;
G4MudulusSimulator* fG4MudulusSimulator;
string fCfgFile;

G4MudulusSimulator::G4MudulusSimulator()
{

}

namespace 
{
	void ProgramUsage() 
	{
		cerr << " Program Usage: " << endl;
		cerr << " ./Mudulusecutable [ -c ConfigFile.json ] " << endl;
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
  cout << "Argc = " << argc << endl;

  Event theEvent;
  fG4MudulusSimulator = new G4MudulusSimulator();
  fG4MudulusSimulator->Initialize(theEvent, fCfgFile);
  fG4MudulusSimulator->RunSimulation(theEvent);
  /*************************************************
    
    Geant4 simulation ended here!
    What happens next is up to you =)

  **************************************************/
  fG4MudulusSimulator->WriteEventInfo(theEvent);
  
  
  return 0;

}

void
G4MudulusSimulator::Initialize(Event& theEvent, string fileName)
{

  cout << "... Initialize ..." << endl;
	// set value of flags according to cfg file
  // reading cfg file using boost
  // eventually as input of executable main(char** fConfig)
  
	fInputFile.clear();
	fOutputFile.clear();

	ptree root;
	read_json(fileName, root);
#warning "JSON parser of input config file should be a method of ConfigManager"
	fInputFile = root.get<string>("InputFile");
	fOutputFile = root.get<string>("OutputFile");
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
G4MudulusSimulator::RunSimulation(Event& theEvent)
{

  SimData& simData = theEvent.GetSimData();
  const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();

  std::cout << "[INFO] Event::SimData: Total number of particles in file = " << NumberOfParticles << std::endl;
  
  if (!NumberOfParticles) {
    std::cerr << "ERROR! No Particles in the Event! Mudulusiting..." << std::endl;
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
  auto fDetConstruction = new G4MudulusDetectorConstruction(theEvent);
  fRunManager->SetUserInitialization(fDetConstruction);
  
  //G4VModularPhysicsList* physicsList = new FTFP_BERT;
  //fRunManager->SetUserInitialization(physicsList);
  fRunManager->SetUserInitialization(new G4MPhysicsList(fPhysicsName));

  G4MudulusPrimaryGeneratorAction *fPrimaryGenerator = new G4MudulusPrimaryGeneratorAction();
  fRunManager->SetUserAction(fPrimaryGenerator);
  
  G4MudulusRunAction *fRunAction = new G4MudulusRunAction();
  fRunManager->SetUserAction(fRunAction);
  
  G4MudulusEventAction *fEventAction = new G4MudulusEventAction();
  fRunManager->SetUserAction(fEventAction);

  fRunManager->SetUserAction(new G4MudulusTrackingAction());

  G4MudulusSteppingAction *fSteppingAction = new G4MudulusSteppingAction(fDetConstruction, fEventAction, theEvent);
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
    G4MudulusSimulator::currentParticle = *it;
    // Run simulation
    fRunManager->BeamOn(1);
    nParticle+=1;

    cout << "Simulated " << nParticle << " particle(s) out of " << NumberOfParticles << endl;
  }


  delete fVisManager;
  delete fRunManager;

  return true;

}


void
G4MudulusSimulator::WriteEventInfo(Event& theEvent)
{
	cout << "... WriteEventInfo ..." << endl;

	cout << "Still nothing to write :(" << endl;


}