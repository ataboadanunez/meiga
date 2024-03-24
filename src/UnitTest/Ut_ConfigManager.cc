#include "Ut_ConfigManager.h"
#include "Utilities.h"
#include "ConfigManager.h"
#include <iostream>
#include <fstream>
#include <cassert>

const string Ut_ConfigManager::cConfigFilename = Utilities::ConcatenatePaths(CONFIG_FILE_PATH, "ExampleG4AppSimulator.json");

bool Ut_ConfigManager::test_ReadConfigurationFile()
{
    // lines below are used to replace the identifier @PATH@ inside the ExampleG4AppSimulator.json by the CONFIG_FILE_PATH
    std::ifstream file(cConfigFilename);
    // read the json file
    std::string jsonString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    // replace identifier inside the file
    string processedString = Utilities::ReplacePlaceHolder(jsonString, "@PATH@", CONFIG_FILE_PATH);
    std::ofstream outFile(cConfigFilename);
    // write json file with the updated field
    outFile << processedString;
    outFile.close();
    
    Event event = ConfigManager::ReadConfigurationFile(cConfigFilename);
    Event::Config &cfg = event.GetConfig();
    int nTestsPassed = 0;
    // input 
    assert(cfg.fInputMode == "UseARTI");
    nTestsPassed++;
    assert(cfg.fInputNParticles == 0);
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

bool Ut_ConfigManager::test_ReadDetectorList(const string &aFilename, Event &theEvent)
{
    return true;
}