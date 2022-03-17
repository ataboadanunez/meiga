/* 
	
	A Meiga application for simulating the Mudulus Detector
  (see G4Models/Mudulus.h)

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
		cerr << " ./exeMeigaApp [ -c ConfigFile.json ] " << endl;
	}

}


int main(int argc, char** argv) 
{

  if (argc < 3) {
    ProgramUsage();
    throw invalid_argument("[ERROR] G4MudulusSimulator::main: A configuration file is needed!");
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
  fG4MudulusSimulator = new G4MudulusSimulator();
  fG4MudulusSimulator->Initialize(theEvent, fCfgFile);
  fG4MudulusSimulator->RunSimulation(theEvent);
  /*************************************************
    
    Geant4 simulation ended here!
    What happens next is up to you =)

  **************************************************/
  fG4MudulusSimulator->WriteEventInfo(theEvent);
  cout << "[INFO] G4MudulusSimulator: End of the run!" << endl;
  time(&end);

  // Calculating total time taken by the program.
  double time_taken = double(end - start);
  cout << "[INFO] G4MudulusSimulator: Time taken by program is : " << fixed
       << time_taken << setprecision(5);
  cout << " sec " << endl;
  
  return 0;

}

void
G4MudulusSimulator::Initialize(Event& theEvent, string fileName)
{

  cout << "[INFO] G4MudulusSimulator::Initialize" << endl;
  cout << "[INFO] G4MudulusSimulator::Initialize: Reading configuration file " << fileName << endl;
  
	fInputFile.clear();
	fOutputFile.clear();

	ptree root;
	read_json(fileName, root);

	fInputFile = root.get<string>("InputFile");
	fOutputFile = root.get<string>("OutputFile");
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
G4MudulusSimulator::RunSimulation(Event& theEvent)
{

  cout << "[INFO] G4MudulusSimulator::RunSimulation" << endl;
  SimData& simData = theEvent.GetSimData();
  cout << "[INFO] G4MudulusSimulator::RunSimulation: Simulation mode selected = " << fSimulationMode << endl;
  const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();

  cout << "[INFO] G4MudulusSimulator::RunSimulation: Total number of particles in file = " << NumberOfParticles << std::endl;
  
  if (!NumberOfParticles) {
    std::cerr << "[ERROR] G4MudulusSimulator::RunSimulation: No Particles in the Event! Exiting." << std::endl;
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
    
    fUImanager->ApplyCommand("/vis/filtering/trajectories/create/chargeFilter");
    fUImanager->ApplyCommand("/vis/filtering/trajectories/chargeFilter-0/add -1");
    fUImanager->ApplyCommand("/vis/filtering/trajectories/chargeFilter-0/add +1");
    //fUImanager->ApplyCommand("/vis/filtering/trajectories/particleFilter-0/invert true");
  }


  // loop over particle vector
  for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
    G4MudulusSimulator::currentParticle = *it;
    // Run simulation
    fRunManager->BeamOn(1);

  }


  delete fVisManager;
  delete fRunManager;

  cout << "[INFO] G4MudulusSimulator::RunSimulation: Geant4 Simulation ended successfully! " << endl;

  return true;

}


void
G4MudulusSimulator::WriteEventInfo(Event& theEvent)
{
	cout << "[INFO] G4MudulusSimulator::WriteEventInfo" << endl;

	cout << "Still nothing to write :(" << endl;


}
