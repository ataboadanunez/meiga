#include "G4MBaseApplication.h"
#include "ConfigManager.h"
#include "DataWriter.h"

void G4MBaseApplication::Initialize(Event &aEvent, std::string aFileName)
{
    // Fill Event object from configuration file
	ConfigManager::ReadConfigurationFile(aEvent, aFileName);
	// get simulation simulation settings
	const Event::Config &cfg = aEvent.GetConfig();
	ConfigManager::PrintConfig(cfg);
	// Read Detector Configuration
	ConfigManager::ReadDetectorList(cfg.fDetectorList, aEvent);
}

void G4MBaseApplication::WriteEventInfo(Event &aEvent)
{
    DataWriter::FileWriter(aEvent);
}
