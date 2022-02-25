/* 
  
  A Meiga application for simulating Water-Cherenkov Detectors

  author: alvaro taboada
  date: 9 Feb 2022

 */

// Headers of this particular application
#include "G4WCDSimulator.h"
#include "G4WCDConstruction.h"
#include "G4WCDPrimaryGeneratorAction.h"
#include "G4WCDStackingAction.h"
#include "G4WCDEventAction.h"
#include "G4WCDRunAction.h"
#include "G4WCDTrackingAction.h"
#include "G4WCDSteppingAction.h"

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

Particle G4WCDSimulator::currentParticle;
G4WCDSimulator* fG4WCDSimulator;
string fCfgFile;

G4WCDSimulator::G4WCDSimulator()
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
    throw invalid_argument("[ERROR] G4WCDSimulator::main: A configuration file is needed!");
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
  fG4WCDSimulator = new G4WCDSimulator();
  fG4WCDSimulator->Initialize(theEvent, fCfgFile);
  fG4WCDSimulator->RunSimulation(theEvent);
  /*************************************************
    
    Geant4 simulation ended here!
    What happens next is up to you =)

  **************************************************/
  fG4WCDSimulator->WriteEventInfo(theEvent);
  cout << "[INFO] G4WCDSimulator: End of the run!" << endl;
  time(&end);

  // Calculating total time taken by the program.
    double time_taken = double(end - start);
    cout << "[INFO] G4WCDSimulator: Time taken by program is : " << fixed
         << time_taken << setprecision(5);
    cout << " sec " << endl;
  
  return 0;

}

void
G4WCDSimulator::Initialize(Event& theEvent, string fileName)
{

  cout << "[INFO] G4WCDSimulator::Initialize" << endl;
  cout << "[INFO] G4WCDSimulator::Initialize: Reading configuration file " << fileName << endl;
  // set value of flags according to cfg file
  // reading cfg file using boost
  // eventually as input of executable main(char** fConfig)
  
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
G4WCDSimulator::RunSimulation(Event& theEvent)
{

  cout << "[INFO] G4WCDSimulator::RunSimulation" << endl;
  SimData& simData = theEvent.GetSimData();
  simData.SetSimulationMode(fSimulationMode);
  cout << "[INFO] G4WCDSimulator::RunSimulation: Simulation mode selected = " << fSimulationMode << endl;
  const unsigned int NumberOfParticles = simData.GetTotalNumberOfParticles();

  cout << "[INFO] G4WCDSimulator::RunSimulation: Total number of particles in file = " << NumberOfParticles << endl;
  
  if (!NumberOfParticles) {
    cerr << "[ERROR] G4WCDSimulator::RunSimulation: No Particles in the Event! Exiting." << endl;
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
  auto fDetConstruction = new G4WCDConstruction(theEvent);
  fRunManager->SetUserInitialization(fDetConstruction);
  
  //G4VModularPhysicsList* physicsList = new FTFP_BERT;
  //fRunManager->SetUserInitialization(physicsList);
  fRunManager->SetUserInitialization(new G4MPhysicsList(fPhysicsName));

  G4WCDPrimaryGeneratorAction *fPrimaryGenerator = new G4WCDPrimaryGeneratorAction(theEvent);
  fRunManager->SetUserAction(fPrimaryGenerator);
  
  G4WCDStackingAction *fStackingAction = new G4WCDStackingAction(theEvent);
  fRunManager->SetUserAction(fStackingAction);

  G4WCDRunAction *fRunAction = new G4WCDRunAction();
  fRunManager->SetUserAction(fRunAction);
  
  G4WCDEventAction *fEventAction = new G4WCDEventAction();
  fRunManager->SetUserAction(fEventAction);

  fRunManager->SetUserAction(new G4WCDTrackingAction());

  G4WCDSteppingAction *fSteppingAction = new G4WCDSteppingAction(fDetConstruction, fEventAction, theEvent);
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
    G4WCDSimulator::currentParticle = *it;
    simData.SetCurrentParticle(*it);
    // Run simulation
    fRunManager->BeamOn(1);
  }


  delete fVisManager;
  delete fRunManager;

  cout << "[INFO] G4WCDSimulator::RunSimulation: Geant4 Simulation ended successfully! " << endl;

  return true;

}


void
G4WCDSimulator::WriteEventInfo(Event& theEvent)
{
  cout << "[INFO] G4WCDSimulator::WriteEventInfo" << endl;

  // for accessing Simulated Data at Detector/Event level
  SimData& simData = theEvent.GetSimData();
  cout << "[INFO] G4WCDSimulator::WriteEventInfo: Accessing DetectorSimData" << endl;
  // loop over detector range
  for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {

    auto& currDet = detIt->second;
    int detId = currDet.GetId();

    DetectorSimData& detSimData = simData.GetDetectorSimData(detId);
    // get number of optical devices in the detector
    int nOptDev = currDet.GetNOptDevice();
    cout << "[INFO] G4WCDSimulator::WriteEventInfo: Accessing data of detector " << detId << " with " << nOptDev << " optical devices." << endl;

    // loop over optical devices
    for (auto odIt = currDet.OptDeviceRange().begin(); odIt != currDet.OptDeviceRange().end(); odIt++) {
      auto& currOd = odIt->second;
      int odId = currOd.GetId();

      OptDeviceSimData& odSimData = detSimData.GetOptDeviceSimData(odId);
      cout << "[INFO] G4WCDSimulator::WriteEventInfo: Accessing signal of " << currOd.GetName() << " " << odId << " from Detector " << detId << endl;

      // checking signal at optical devices
      const auto *peTimeDistributionRange = odSimData.PETimeDistributionRange();
      if (!peTimeDistributionRange) {
        cerr << "No Time for this channel!" << endl;
        continue;
      }

      cout << endl;
      // accessing signals for different particle components

      for (int compIt = Particle::eElectromagnetic; compIt < Particle::eEnd; compIt++) {
        
        Particle::Component particleComponent = static_cast<Particle::Component>(compIt);
        // using currentParticle to get the component name. its ugly but should do the trick
        auto componentName = G4WCDSimulator::currentParticle.GetComponentName(particleComponent);
        cout << "[INFO] G4WCDSimulator::WriteEventInfo: Accessing signals of particle component " << componentName << endl;
        try 
        { 
          const auto peTimeDistributionRangeComp = odSimData.PETimeDistributionRange(particleComponent);
          if (!peTimeDistributionRange) {
            cerr << "No time distribution for particle component " << componentName << endl;
            continue;
          }

          ofstream* fPEFile = new ofstream();
          fPEFile->open("photoelectrons_"+std::to_string(detId)+"_PMT_"+std::to_string(odId)+"_"+std::to_string(particleComponent)+".dat");
          for (auto peTime = peTimeDistributionRangeComp.begin(); peTime != peTimeDistributionRangeComp.end(); ++peTime) {
            size_t npe = peTime->size();
            (*fPEFile) << npe << " ";
          }
          
          (*fPEFile) << endl;
          cout << "[INFO] G4WCDSimulator::WriteEventInfo: Signals coppied!" << endl;
          fPEFile->close();
        }
        catch (std::out_of_range &e)
        {
          cerr << "No time distribution for particle component " << componentName << "! (Exception: " << e.what() << " )." << endl;
        }
        
        
      
      }


    }


  }

}
