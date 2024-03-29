#include "DefaultProperties.h"
#include "ConfigManager.h"
#include "Utilities.h"

const ptree& empty_ptree(){
	static ptree t;
	return t;
}

const std::string cDetectorPropertiesFile = Utilities::ConcatenatePaths(CONFIG_FILE_PATH, "DetectorProperties.xml");

void
DefaultProperties::SetDefaultProperties()
{
	
	string filename = cDetectorPropertiesFile;

	
	if (filename.empty()) 
		return;
	std::cout << "[INFO] DefaultProperties::SetDefaultProperties: Setting default properties from file " << filename << std::endl;
	// read xml and set parameters
	ptree tree;
	read_xml(filename, tree);
	string branchName = "detectorProperties";
	// setting default detector properties from XML
	gTankRadius = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "tankRadius");
	gTankHeight = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "tankHeight");
	gTankThickness = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "tankThickness");
	gImpuritiesFraction = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "impuritiesFraction", false);
	
	gNumberOfBars = ConfigManager::GetPropertyFromXML<int>(tree, branchName, "numberOfBars", false);
	gNumberOfPanels = ConfigManager::GetPropertyFromXML<int>(tree, branchName, "numberOfPanels", false);
	gBarWidth = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "barWidth");
	gBarLength = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "barLength");
	gBarThickness = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "barThickness");
	gCoatingThickness = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "coatingThickness");
	gCasingThickness = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "casingThickness");
	gRotationAngle = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "rotationAngle", false);
	gFiberLength = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "fiberLength");
	gFiberRadius = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "fiberRadius");
	gCladdingThickness = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "claddingThickness");
	
	gLength = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "length");
	gWidth = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "width");
	gThickness = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "thickness");
	gDistancePanels = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "distancePanels");
	
	// ground size parameters
	gGroundSizeX = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "groundSizeX");
	gGroundSizeY = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "groundSizeY");
	gGroundSizeZ = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "groundSizeZ");

}