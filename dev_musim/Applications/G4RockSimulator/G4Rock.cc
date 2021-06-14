/* main script of a simple application to simulate charged
   particles traversing a block of rock

   author: alvaro
   date: 17.05.21
 */

#include "G4RunManagerFactory.hh"

#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4RockDetectorConstruction.h"
#include "G4RockPhysicsList.h"
#include "G4RockActionInitialization.h"

#include "Randomize.hh"
#include <CorsikaUtilities.h>

using namespace std;

int main(int argc,char** argv) {
  /* Note on the following variables:
  these should be declared in a header file,
  not here (G4Rock.h), initialized in the
  (non-existing yet) class constructor and
  managed by a configuration file!

  */

  int fVerbosity = 1;
  bool fGeoVisOn = false;
  bool fTrajVisOn = false;
  // check physicsList etc etc etc
  string physicsName = "QGSP_BERT_HP";
  string fRenderFile = "VRML2FILE";
  G4VisManager* fVisManager = nullptr;


  // construct the default run manager
  auto fRunManager = G4RunManagerFactory::CreateRunManager();

  // set mandatory initialization classes
  auto fDetConstruction = new G4RockDetectorConstruction();
  fRunManager->SetUserInitialization(fDetConstruction);
  
  fRunManager->SetUserInitialization(new G4RockPhysicsList(physicsName));
  fRunManager->SetUserInitialization(new G4RockActionInitialization(fDetConstruction));

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
  fRunManager->BeamOn(10000);

  // job termination
  delete fVisManager;
  delete fRunManager;
  return 0;

}
/*
void
ParticleInjector::ReadParticleFile()
{	

	G4cout << "[DEBUG] ParticleInjector: Running ReadParticleFile()" << G4endl;
	string line;
	do {

    if (!getline(*fInputStream, line)) {
      fInputStream->close();
    }

    istringstream iss(line);
    
    if (line.find("#") == false){
    	cout << line << endl;
    	continue;
    }
  	
  	cout << line << endl;
    if (!(iss >> *fParticleId
              >> px >> py >> pz
              >> x >> y
              >> z
              >> primaryShowerId
              >> primaryParticleId
              >> primaryEnergy
              >> primaryTheta
              >> primaryPhi)){
              //>> firstInteraction)) {
      cout << "Malformed line." << endl;
    }

  } while (*fParticleId == 201 || *fParticleId == 301);  // for G4's sake skip Deuterium or Tritium 
}
*/
