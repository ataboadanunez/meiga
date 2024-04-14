#include "Event.h"
#include "Utilities.h"
#include "ConfigManager.h"
#include "G4MDetectorConstruction.h"
#include "G4MPhysicsList.h"
#include "G4MPrimaryGeneratorAction.h"

// Geant4 headers
#include "G4RunManagerFactory.hh"
#include "Randomize.hh"

// c++ headers
#include <iostream>


const string cConfigFilename = Utilities::ConcatenatePaths(CONFIG_FILE_PATH, "Ut_G4AppSimulator.json");
const string cDetectorFilename = Utilities::ConcatenatePaths(CONFIG_FILE_PATH, "Ut_DetectorList.xml");

namespace
{   
    Event test_ReadConfigurationFile() {
    
        Event event = ConfigManager::ReadConfigurationFile(cConfigFilename);
        Event::Config &cfg = event.GetConfig();
        int nTestsPassed = 0;
        // input 
        assert(cfg.fInputMode == "UseEcoMug");
        nTestsPassed++;
        assert(cfg.fInputNParticles == 1);
        nTestsPassed++;
        // output
        assert(cfg.fCompressOutput == true);
        nTestsPassed++;
        assert(cfg.fSavePETimeDistribution == true);
        nTestsPassed++;
        assert(cfg.fSaveComponentsPETimeDistribution == false);
        nTestsPassed++;
        assert(cfg.fSaveEnergy == false);
        nTestsPassed++;
        assert(cfg.fSaveComponentsEnergy == false);
        nTestsPassed++;
        // simulation
        assert(cfg.fSimulationMode == "eFast");
        nTestsPassed++;
        assert(cfg.fGeoVis == false);
        nTestsPassed++;
        assert(cfg.fTrajVis == false);
        nTestsPassed++;
        assert(cfg.fCheckOverlaps == false);
        nTestsPassed++;
        std::cout << "[INFO] test_ReadConfigurationFile(): " << nTestsPassed << " / " << nTestsPassed << " PASSED" << std::endl; 
        
        return event;
    }

    SimData& test_ReadDetectorList(Event &aEvent) {

        ConfigManager::ReadDetectorList(cDetectorFilename, aEvent);
        SimData &simData = aEvent.GetSimData();
        assert(simData.GetInjectionMode() == SimData::InjectionMode::eVertical);
        const std::vector<double> &injectionOrigin = simData.GetInjectionOrigin();
        assert(injectionOrigin.at(0) == 0.0 * CLHEP::m);
        assert(injectionOrigin.at(1) == 0.0 * CLHEP::m);
        assert(injectionOrigin.at(2) == 1.0 * CLHEP::m);
        // get instances of Detector with ID 0
        Detector detector = aEvent.GetDetector(0);
        assert(detector.isValid());
        assert(detector.GetType() == Detector::eScintillator);
        std::vector<double> detPosition = detector.GetDetectorPosition();
        assert(detPosition.at(0) == -0.01 * CLHEP::m);
        assert(detPosition.at(1) == 0.02 * CLHEP::m);
        assert(detPosition.at(2) == 0.5 * CLHEP::m);
        assert(detector.GetNBars() == 8);
        
        return simData;
    }

    void test_PrimaryGenerator(SimData& aSimData) {
        
        Particle currentParticle = aSimData.GetCurrentParticle();
        assert(std::abs(currentParticle.GetParticleId()) == 13);
        const std::vector<double> &injectionPosition = currentParticle.GetInjectionPosition();
        assert(injectionPosition.at(0) == 0.0);
        assert(injectionPosition.at(1) == 0.0);
        assert(injectionPosition.at(2) == 0.0);
        const std::vector<double> &momentumDirection = currentParticle.GetMomentumDirection();
        assert(momentumDirection.at(0) == 0.0);
        assert(momentumDirection.at(1) == 0.0);
        assert(momentumDirection.at(2) == -449.16473577427365);
        assert(currentParticle.GetZenith() == 2.4114694941536605);
        assert(currentParticle.GetAzimuth() == 3.5926461359435269);
    }
}

int main(int argc, char** argv)
{
    Event event = test_ReadConfigurationFile();
    Event::Config &cfg = event.GetConfig();
    SimData &simData = test_ReadDetectorList(event);
    
    // prepare simulation
    G4long seed = 123456789;
    simData.SetSeed(seed);
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(seed);
    G4RunManager *runManager = G4RunManagerFactory::CreateRunManager();

    if(!runManager) {
        return 1;
    }

    G4MDetectorConstruction *detConstruction = new G4MDetectorConstruction(event);
    if(!detConstruction) {
        return 1;
    }
    runManager->SetUserInitialization(detConstruction);
    runManager->SetUserInitialization(new G4MPhysicsList(simData.GetPhysicsListName()));

    G4MPrimaryGeneratorAction *prmGenAction = new G4MPrimaryGeneratorAction(event);
    if(!prmGenAction) {
        return 1;
    }
    runManager->SetUserAction(prmGenAction);

    // initialize G4 kernel
    runManager->Initialize();
    for(auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
        runManager->BeamOn(1);
        test_PrimaryGenerator(simData);
    }
    // need another test for output 

    return 0;
}