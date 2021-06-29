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
#include "SimData.h"
#include "ReadParticleFile.h"


#include <string>

using namespace std;

Particle G4RockSimulator::currentParticle;
//G4RockSimulator::fOutputFile->open("SiPMTraces2.dat");

G4RockSimulator::G4RockSimulator() 
  //fOutputFile(new ofstream())
{
	 //fOutputFile->open("SiPMTraces2.dat");
  
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

  // TODO: INCLUIR RANDOM SEED!!!
  G4long myseed = time(NULL);
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  G4Random::setTheSeed(myseed);
  G4cout << "Seed for random generation: " << myseed << G4endl;

  string physicsName = "QGSP_BERT_HP";
  string fRenderFile = "VRML2FILE";
  /*
    ToDo:
    CHECK PhysicsList 
    CREATE ERROR LOGGER
    CREATE CONFIG FILE READER
  */

  ofstream* fOutputFile = new ofstream();
  fOutputFile->open("SiPMTraces_reflections.dat");

  // create Event from EventFileReader
  // mover input file a cfg
  Event theEvent = ReadParticleFile::EventFileReader("/home/alvaro/data/sims/sierra_grande/sample_muons2.txt");

  SimData& simData = theEvent.GetSimData();
  const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();

  std::cout << "[INFO] Event::SimData: Total number of particles in file = " << NumberOfParticles << std::endl;
  
  if (!NumberOfParticles) {
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
  
  for (auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
    G4RockSimulator::currentParticle = *it;
	 //G4I->setParticle(*it); 
	 fRunManager->BeamOn(1);
  }

  /*
  std::vector<std::vector<double>*>* trazas = fEventAction->GetTrazas();
  for (unsigned int i=0; i<trazas->size(); i++) {
    //G4cout << "Evento " << i << G4endl;
    for (unsigned int j=0; j<trazas->at(i)->size(); j++)
    G4cout << trazas->at(i)->at(j) << " ";
  }
  */

  std::vector<double> peTimes = simData.GetPhotonTime();
  for (unsigned int i=0; i<peTimes.size(); i++)
    G4cout << peTimes[i] << " ";
  
  //Particle& currentP = G4RockSimulator::currentParticle;
	// job termination
  
  delete fVisManager;
  delete fRunManager;
  fOutputFile->close();
  return 0;

}
