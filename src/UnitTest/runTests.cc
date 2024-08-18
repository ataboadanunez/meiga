#include "Event.h"
#include "Utilities.h"
#include "ParticleInjection.h"
#include "ConfigManager.h"
#include "G4MDetectorConstruction.h"
#include "G4MPhysicsList.h"
#include "G4MPrimaryGeneratorAction.h"
#include "ConfigFilePath.h"
#include "Scintillator.h"

// Geant4 headers
#include "G4RunManagerFactory.hh"
#include "Randomize.hh"

// c++ headers
#include <iostream>


const string cConfigFilename = Utilities::ConcatenatePaths(GetConfigFilePath(), "Ut_G4AppSimulator.json");
const string cDetectorFilename = Utilities::ConcatenatePaths(GetConfigFilePath(), "Ut_DetectorList.xml");

namespace
{   
    double threshold = 1e-10;
    double sanitizeZero(double value) {
        return (std::abs(value) < threshold) ? 0.0 : value;
    }

    void compareValues(double expected, double actual, bool debug=false) {
        if (debug) {
            std::cout << "Expected: " << std::fixed << std::setprecision(15) << expected << std::endl;
            std::cout << "Actual  : " << std::fixed << std::setprecision(15) << actual << std::endl;
        }
        // adjust tolerance as needed
        assert(std::abs(expected - actual) < 1e-12);
    }

    Event test_ReadConfigurationFile() {
    
        Event event;
        ConfigManager::ReadConfigurationFile(event, cConfigFilename);
        Event::Config &cfg = event.GetConfig();
        // input 
        assert(cfg.fInputMode == "UseEcoMug");
        assert(cfg.fInputNParticles == 1);
        // output
        assert(cfg.fCompressOutput == true);
        assert(cfg.fSavePETimeDistribution == true);
        assert(cfg.fSaveComponentsPETimeDistribution == false);
        assert(cfg.fSaveEnergy == false);
        assert(cfg.fSaveComponentsEnergy == false);
        // simulation
        assert(cfg.fSimulationMode == "eFast");
        assert(cfg.fGeoVis == false);
        assert(cfg.fTrajVis == false);
        assert(cfg.fCheckOverlaps == false);
        Logger::Print("PASS", INFO, "test_ReadConfigurationFile");
        
        return event;
    }

    SimData& test_ReadDetectorList(Event &aEvent) {

        ConfigManager::ReadDetectorList(cDetectorFilename, aEvent);
        SimData &simData = aEvent.GetSimData();
        ParticleInjection &injection = simData.GetParticleInjection();
        assert(injection.IsValid());
        assert(injection.GetInjectionType() == ParticleInjection::eVertical);
        const std::vector<double> &injectionOrigin = injection.GetInjectionOrigin();
        assert(injectionOrigin.at(0) == 0.0 * CLHEP::m);
        assert(injectionOrigin.at(1) == 0.0 * CLHEP::m);
        assert(injectionOrigin.at(2) == 1.0 * CLHEP::m);
        // get instances of Detector with ID 0
        Detector& detector = aEvent.GetDetector(0);
        assert(detector.isValid());
        assert(detector.GetType() == Detector::eScintillator);
        std::vector<double> detPosition = detector.GetDetectorPosition();
        assert(detPosition.at(0) == -0.01 * CLHEP::m);
        assert(detPosition.at(1) == 0.02 * CLHEP::m);
        assert(detPosition.at(2) == 0.5 * CLHEP::m);
        Scintillator *scinDet = dynamic_cast<Scintillator*>(&detector);
        assert(scinDet);
        int nBars = scinDet->GetNBars();
        assert(nBars == 8);
        Logger::Print("PASS", INFO, "test_ReadDetectorList");

        return simData;
    }

    void test_PrimaryGenerator(SimData& aSimData) {
        
        Particle currentParticle = aSimData.GetCurrentParticle();
        assert(std::abs(currentParticle.GetParticleId()) == 13);
        const std::vector<double> &injectionPosition = currentParticle.GetInjectionPosition();
        compareValues(0.0, sanitizeZero(injectionPosition.at(0)) / CLHEP::m);
        compareValues(0.0, sanitizeZero(injectionPosition.at(1)) / CLHEP::m);
        compareValues(1.0, sanitizeZero(injectionPosition.at(2)) / CLHEP::m);
        const std::vector<double> &momentumDirection = currentParticle.GetMomentumDirection();
        compareValues(0.0, momentumDirection.at(0));
        compareValues(0.0, momentumDirection.at(1));
        compareValues(-837.874689031361299, momentumDirection.at(2));
        compareValues(2.257155077674716, currentParticle.GetZenith());
        compareValues(1.022436562227548, currentParticle.GetAzimuth());
        
        Logger::Print("PASS", INFO, "test_PrimaryGenerator");
    }
}

int main(int argc, char** argv)
{
    int nTestsPassed = 0;
    Event event = test_ReadConfigurationFile();
    nTestsPassed++;
    Event::Config &cfg = event.GetConfig();
    SimData &simData = test_ReadDetectorList(event);
    nTestsPassed++;
    
    // prepare simulation
    G4long seed = 123456789;
    simData.SetSeed(seed);
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(seed);
    G4RunManager *runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
    runManager->SetVerboseLevel(0);
    
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
    nTestsPassed++;
    Logger::Print("All Unit Tests passed.", INFO);
    // need another test for output 
    delete runManager;
    return 0;
}