/* main script of a simple application to simulate charged
   particles traversing a block of rock

   author: alvaro
   date: 17.05.21

   $Id:$
 */

#include "G4RockSimulator.h"
#include "G4RockDetectorConstruction.h"
#include "G4RockPhysicsList.h"
#include "G4RockPrimaryGeneratorAction.h"
#include "G4RockEventAction.h"
#include "G4RockRunAction.h"
#include "G4RockSteppingAction.h"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"
#include "CorsikaUtilities.h"
#include "Event.h"
#include "Particle.h"

#include <string>

using namespace std;

Particle G4RockSimulator::currentParticle;
//G4RockSimulator::fOutputFile->open("SiPMTraces2.dat");

G4RockSimulator::G4RockSimulator()
  //fOutputFile(new ofstream())
{
	 
}

G4RockSimulator::~G4RockSimulator()
{
  //fOutputFile->close();
}

int 
main(/*int argc, char** argv*/) {
  
  // estas variables deben ser leidas desde un archivo de configuracion!
  int fVerbosity = 1;
  bool fGeoVisOn = false;
  bool fTrajVisOn = false;
  // check physicsList etc etc etc
  string physicsName = "QGSP_BERT_HP";
  string fRenderFile = "VRML2FILE";
  /*
    ToDo:
    CREATE ERROR LOGGER
    PRINT INPUT VARIABLES / INFO ON SCREEN
  */

  ofstream* fOutputFile = new ofstream();
  fOutputFile->open("SiPMTraces2.dat");

  Event theEvent = Event::ReadParticleFile("/home/alvaro/data/sims/sierra_grande/muons_sierra_grande.shw");

  if (!theEvent.GetTotalNumberOfParticles()) {
    std::cerr << "ERROR! No Particles in the Event! Exiting..." << std::endl;
    return -1;
  }

  G4VisManager* fVisManager = nullptr;
  
  // construct the default run manager
  auto fRunManager = G4RunManagerFactory::CreateRunManager();

  // set mandatory initialization classes
  auto fDetConstruction = new G4RockDetectorConstruction();
  fRunManager->SetUserInitialization(fDetConstruction);
  
  fRunManager->SetUserInitialization(new G4RockPhysicsList(physicsName));
  
  G4RockPrimaryGeneratorAction *fPrimaryGenerator = new G4RockPrimaryGeneratorAction();
  fRunManager->SetUserAction(fPrimaryGenerator);
  
  G4RockRunAction *fRunAction = new G4RockRunAction();
  fRunManager->SetUserAction(fRunAction);
  
  G4RockEventAction *fEventAction = new G4RockEventAction(fRunAction, fPrimaryGenerator);
  fRunManager->SetUserAction(fEventAction);

  G4RockSteppingAction *fSteppingAction = new G4RockSteppingAction(fDetConstruction, fEventAction, fOutputFile);
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
    //fUImanager->ApplyCommand("/vis/open "); // for now hardcoded to this render type
    fUImanager->ApplyCommand("/vis/scene/create");
    fUImanager->ApplyCommand("/vis/sceneHandler/attach");
    fUImanager->ApplyCommand("/vis/scene/add/volume");
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

  //UI->ApplyCommand("/run/verbose 1");
  //UI->ApplyCommand("/event/verbose 1");
  
  // start a run 
  //int numberOfEvent = 3;
  
  //ReadParticleFile();
  
  for (auto it = theEvent.GetParticleVector().begin(); it != theEvent.GetParticleVector().end(); ++it) {
    G4RockSimulator::currentParticle = *it;
	 //G4I->setParticle(*it); 
	 fRunManager->BeamOn(1);
  }
  
  //Particle& currentP = G4RockSimulator::currentParticle;
	  // job termination
  delete fVisManager;
  delete fRunManager;
  return 0;

}
