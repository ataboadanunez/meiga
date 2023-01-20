#include "DefaultProperties.h"
#include "ConfigManager.h"


const ptree& empty_ptree(){
	static ptree t;
	return t;
}


void
DefaultProperties::SetDefaultProperties(const string &filename)
{
	if (filename.empty()) 
		return;

	// read xml and set parameters
	ptree tree;
	read_xml(filename, tree);
	string branchName = "detectorProperties";
	// setting default detector properties from XML
	gTankRadius = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "tankRadius");
	gTankHeight = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "tankHeight");
	gTankThickness = ConfigManager::GetPropertyFromXML<double>(tree, branchName, "tankThickness");
	
	gNumberOfBars = ConfigManager::GetPropertyFromXML<int>(tree, branchName, "barsInPanel", false);
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