/* 
	
	Main script of an example application using Geant4

	author: alvaro taboada
	date: 15 Sep 2021

	$Id:$

*/

//Meiga files ended with .h and G4 files with .hh

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
#include "FTFP_BERT.hh" //Physics list
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"

// Framework libraries
#include "CorsikaUtilities.h"
#include "ReadParticleFile.h"
#include "Event.h"
#include "SimData.h"
#include "SiPMSimData.h"
#include "Detector.h"
#include "Module.h"
#include "SiPM.h"

// boost libraries
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace boost::property_tree;
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
  cout << "Argc = " << argc << endl;

  Event theEvent;
  fG4CasposoSimulator = new G4CasposoSimulator();
  fG4CasposoSimulator->Initialize(theEvent, fCfgFile);
  fG4CasposoSimulator->RunSimulation(theEvent);
  /*************************************************
    
    Geant4 simulation ended here!
    What happens next is up to you =)

  **************************************************/
  fG4CasposoSimulator->WriteEventInfo(theEvent);
  
  
  return 0;

}

void
G4CasposoSimulator::Initialize(Event& theEvent, string fileName)
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
G4CasposoSimulator::RunSimulation(Event& theEvent)
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
  auto fDetConstruction = new G4CasposoDetectorConstruction(theEvent);
  fRunManager->SetUserInitialization(fDetConstruction);
  
  fRunManager->SetUserInitialization(new G4CasposoPhysicsList(fPhysicsName));  
 
  G4CasposoPrimaryGeneratorAction *fPrimaryGenerator = new G4CasposoPrimaryGeneratorAction();
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

  int nParticle = 0;
  // loop over particle vector
  for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
    G4CasposoSimulator::currentParticle = *it;
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
G4CasposoSimulator::WriteEventInfo(Event& theEvent)
{
	cout << "... WriteEventInfo ..." << endl;

	cout << "Still nothing to write :(" << endl;


}
