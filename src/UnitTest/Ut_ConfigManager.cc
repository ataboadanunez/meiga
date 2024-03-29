#include "Ut_ConfigManager.h"
#include "Utilities.h"
#include "ConfigManager.h"
#include <iostream>
#include <fstream>
#include <cassert>

const string cConfigFilename = Utilities::ConcatenatePaths(CONFIG_FILE_PATH, "Ut_G4AppSimulator.json");
const string cDetectorFilename = Utilities::ConcatenatePaths(CONFIG_FILE_PATH, "Ut_DetectorList.xml");

bool Ut_ConfigManager::test_ReadConfigurationFile()
{
    
    Event event = ConfigManager::ReadConfigurationFile(cConfigFilename);
    Event::Config &cfg = event.GetConfig();
    int nTestsPassed = 0;
    // input 
    assert(cfg.fInputMode == "UseEcoMug");
    nTestsPassed++;
    assert(cfg.fInputNParticles == 5);
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
    std::cout << "[INFO] Ut_ConfigManager::test_ReadConfigurationFile(): " << nTestsPassed << " / " << nTestsPassed << " PASSED" << std::endl; 
    return true;
}

bool Ut_ConfigManager::test_ReadDetectorList()
{
    
    Event theEvent;
    ConfigManager::ReadDetectorList(cDetectorFilename, theEvent);
    SimData &simData = theEvent.GetSimData();
    assert(simData.GetInjectionMode() == SimData::InjectionMode::eVertical);
    const std::vector<double> &injectionOrigin = simData.GetInjectionOrigin();
    assert(injectionOrigin.at(0) == 0.0 * CLHEP::m);
    assert(injectionOrigin.at(1) == 0.0 * CLHEP::m);
    assert(injectionOrigin.at(2) == 1.0 * CLHEP::m);
    // get instances of Detector with ID 0
    Detector detector = theEvent.GetDetector(0);
    assert(detector.isValid());
    assert(detector.GetType() == Detector::eScintillator);
    std::vector<double> detPosition = detector.GetDetectorPosition();
    assert(detPosition.at(0) == -0.01 * CLHEP::m);
    assert(detPosition.at(1) == 0.02 * CLHEP::m);
    assert(detPosition.at(2) == 0.5 * CLHEP::m);
    assert(detector.GetNBars() == 8);
    
    return true;
}