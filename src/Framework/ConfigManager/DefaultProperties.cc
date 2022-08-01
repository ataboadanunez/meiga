#include "DefaultProperties.h"

#include "G4UnitsTable.hh"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

using boost::property_tree::ptree;
using namespace std;


const ptree& empty_ptree(){
	static ptree t;
	return t;
}

template <typename T>
inline static T GetPropertyFromXML(const ptree &tree, const string &branchName, const string &property, bool hasUnit = true) {
  T res = tree.get<T>(branchName+"."+property);
  if (hasUnit) {
  	auto unit = G4UnitDefinition::GetValueOf(tree.get<string>(branchName+"."+property+".<xmlattr>.unit"));
  	res *= unit;
  } 
  	

  return res;
}

template <typename T>
inline static T GetPropertyFromXML(const ptree &tree, const string &branchName, const string &property, const T &defaultVal, bool hasUnit = true, const string &defaultUnit = "mm") {
  T res = tree.get<T>(branchName+"."+property, defaultVal);
  if (hasUnit) {
  	auto unit = G4UnitDefinition::GetValueOf(tree.get<string>(branchName+"."+property+".<xmlattr>.unit", defaultUnit));
  	res *= unit;
  } 
  	

  return res;
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
	gTankRadius = GetPropertyFromXML<double>(tree, branchName, "tankRadius");
	gTankHeight = GetPropertyFromXML<double>(tree, branchName, "tankHeight");
	gTankThickness = GetPropertyFromXML<double>(tree, branchName, "tankThickness");
	
	gNumberOfBars = GetPropertyFromXML<int>(tree, branchName, "barsInPanel", false);
	gBarWidth = GetPropertyFromXML<double>(tree, branchName, "barWidth");
	gBarLength = GetPropertyFromXML<double>(tree, branchName, "barLength");
	gBarThickness = GetPropertyFromXML<double>(tree, branchName, "barThickness");
	gCoatingThickness = GetPropertyFromXML<double>(tree, branchName, "coatingThickness");
	gCasingThickness = GetPropertyFromXML<double>(tree, branchName, "casingThickness");
	gFiberLength = GetPropertyFromXML<double>(tree, branchName, "fiberLength");
	gFiberRadius = GetPropertyFromXML<double>(tree, branchName, "fiberRadius");
	gCladdingThickness = GetPropertyFromXML<double>(tree, branchName, "claddingThickness");
	


}