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
}

void G4MBaseApplication::WriteEventInfo(Event &aEvent)
{
    DataWriter::FileWriter(aEvent);
}

void G4MBaseApplication::SetupManagers(Event &aEvent, G4UImanager &aUIManager, G4VisManager &aVisManager)
{
	const Event::Config &cfg = aEvent.GetConfig();
	switch (cfg.fVerbosity) {
		case 1:
			aUIManager.ApplyCommand("/run/verbose 1");
			aUIManager.ApplyCommand("/event/verbose 0");
			aUIManager.ApplyCommand("/tracking/verbose 0");
			break;
		case 2:
			aUIManager.ApplyCommand("/run/verbose 1");
			aUIManager.ApplyCommand("/event/verbose 1");
			aUIManager.ApplyCommand("/tracking/verbose 0");
			break;
		case 3:
			aUIManager.ApplyCommand("/run/verbose 1");
			aUIManager.ApplyCommand("/event/verbose 1");
			aUIManager.ApplyCommand("/tracking/verbose 1");
			break;
		default:
			aUIManager.ApplyCommand("/run/verbose 0");
			aUIManager.ApplyCommand("/event/verbose 0");
			aUIManager.ApplyCommand("/tracking/verbose 0");
	}
	if(cfg.fGeoVis || cfg.fTrajVis) {
		aVisManager.Initialize();
	}
	aUIManager.ApplyCommand(("/vis/open " + cfg.fRenderFile).c_str());
	aUIManager.ApplyCommand("/vis/scene/create");
	aUIManager.ApplyCommand("/vis/sceneHandler/attach");
	aUIManager.ApplyCommand("/vis/scene/add/volume");
	aUIManager.ApplyCommand("/vis/scene/add/axes 0 0 0 1 m");
	aUIManager.ApplyCommand("/vis/viewer/set/viewpointThetaPhi 0. 0.");
	aUIManager.ApplyCommand("/vis/viewer/set/targetPoint 0 0 0");
	aUIManager.ApplyCommand("/vis/viewer/zoom 1");
	aUIManager.ApplyCommand("/vis/viewero/set/style/wireframe");
	aUIManager.ApplyCommand("/vis/drawVolume");
	aUIManager.ApplyCommand("/vis/scene/notifyHandlers");
	aUIManager.ApplyCommand("/vis/viewer/update");

	if (cfg.fTrajVis) {
		aUIManager.ApplyCommand("/tracking/storeTrajectory 1");
		aUIManager.ApplyCommand("/vis/scene/add/trajectories");
	}
}
