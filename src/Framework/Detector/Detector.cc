#include "Detector.h"

#include "WCD.h"
#include "Scintillator.h"
#include "Musaic.h"
#include "Mudulus.h"

#include "G4UnitsTable.hh"

#include <map>
#include <string>
#include <iostream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

using std::map;
using namespace std;
using boost::property_tree::ptree;

// function to be used when calling a detector type from an XML file
Detector::DetectorType
Detector::StringToType(string name)
{
	if (name == "eWCD")
		return Detector::eWCD;
	else if (name == "eScintillator")
		return Detector::eScintillator;
	else if (name == "eMusaic")
		return Detector::eMusaic;
	else if (name == "eMudulus")
		return Detector::eMudulus;
	else
		return Detector::eUnknown;
}


Detector::Detector(const unsigned int id, DetectorType type) : 
	fDetectorId(id),
	fType(type)
{
	SetDetectorProperties(type);
}


// this is used to map from the Detector type to its corresponding BuildDetector function in G4Models
static map<enum Detector::DetectorType, void(*)(G4LogicalVolume*, Detector&, Event&, G4bool)> TypeToBuild;

static void 
InitTypeToBuild() {
	static bool isInitialized = false;
	if (isInitialized) 
		return;
	// fills map with corresponding BuildDetector function
	isInitialized = true;
	TypeToBuild[Detector::eWCD]			     = WCD::BuildDetector;
	TypeToBuild[Detector::eScintillator] = Scintillator::BuildDetector;
	TypeToBuild[Detector::eMusaic]       = Musaic::BuildDetector;
	TypeToBuild[Detector::eMudulus]      = Mudulus::BuildDetector;

}

void BuildDetector(G4LogicalVolume *logMother, Detector &det, Event &evt, G4bool overlaps) {
	
	// initialize map
	InitTypeToBuild();
	
	try {
		TypeToBuild[det.GetType()](logMother, det, evt, overlaps);
	}
	catch (std::out_of_range &e) {
		std::cerr << "No BuildDetector for DetectorType " << det.GetType() << std::endl;
	}
}

void
Detector::MakeOptDevice(int id, OptDevice::DeviceType type)
{
	fOptDeviceMap.emplace(id, OptDevice(id, type));
	fNOptDevices = fOptDeviceMap.size();
}

bool
Detector::HasOptDevice(int id)
{
	return fOptDeviceMap.find(id) != fOptDeviceMap.end();
}

void
Detector::SetLogicalVolume(string volName, G4LogicalVolume* log)
{
	fLogicalVolumeMap.emplace(volName, log);
}

bool
Detector::HasLogicalVolume(string volName)
{	
	return fLogicalVolumeMap.find(volName) != fLogicalVolumeMap.end();
}

void
Detector::SetDetectorProperties(const DetectorType type, string detList)
{
	// set default values for different detector types
	// read XML with detector parameters
	// NOTE: there should be a better way to implement 
	// the detector configuration, for example writing a function
	// that gets the specific tag of the XML for the desired parameter
	// e.g., SetRadius() { fRadius = GetData("tankRadius"); }

	ptree tree;
	
	cout << "[INFO] Detector::SetDetectorProperties: Reading detector properties from file = " << detList << endl;
	read_xml(detList, tree);
	for (const auto& i : tree.get_child("detectorList")) {
		ptree subtree;
		string name;
		tie(name, subtree) = i;

		if (name != "detector")
			continue;

		string detTypeStr = subtree.get<string>("<xmlattr>.type");
		Detector::DetectorType detType = Detector::StringToType(detTypeStr);

		if (type != detType)
			continue;

		cout << "[INFO] Detector::SetDetectorProperties: Detector type " << detType << endl;

		for (const auto& v : subtree.get_child("")) {
			string xmlLabel = v.first;
			if (xmlLabel != "<xmlattr>") {
				string xmlValue = v.second.data();
				// loop over XML tags to set the corresponding value

				// WCD parameters
				if (xmlLabel == "tankRadius") {
					double value = stod(xmlValue);
					double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
					SetTankRadius(value * unit);
				}
				else if (xmlLabel == "tankHeight") {
					double value = stod(xmlValue);
					double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
					SetTankHeight(value * unit);
				}
				else if (xmlLabel == "tankThickness") {
					double value = stod(xmlValue);
					double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
					SetTankThickness(value * unit);
				}

				// scintillator detectors
				else if (xmlLabel == "barsInPanel") {
					int value = stoi(xmlValue);
					SetNBars(value);
				}
				else if (xmlLabel == "barLength") {
					double value = stod(xmlValue);
					double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
					SetBarLength(value * unit);
				}
				else if (xmlLabel == "barWidth") {
					double value = stod(xmlValue);
					double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
					SetBarWidth(value * unit);
				}
				else if (xmlLabel == "barThickness") {
					double value = stod(xmlValue);
					double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
					SetBarThickness(value * unit);
				}
				else if (xmlLabel == "coatingThickness") {
					double value = stod(xmlValue);
					double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
					SetBarCoatingThickness(value * unit);
				}
				else if (xmlLabel == "casingThickness") {
					double value = stod(xmlValue);
					double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
					SetCasingThickness(value * unit);
				}
				else if (xmlLabel == "fiberLength") {
					double value = stod(xmlValue);
					double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
					SetFiberLength(value * unit);
				}
				else if (xmlLabel == "fiberRadius") {
					double value = stod(xmlValue);
					double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
					SetFiberRadius(value * unit);
				}
				else if (xmlLabel == "claddingThickness") {
					double value = stod(xmlValue);
					double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
					SetCladdingThickness(value * unit);
				}
				
					
			}
		}

	}

}

