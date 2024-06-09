#include "Detector.h"
#include "Logger.h"

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

Detector::Detector(const int id, const DetectorType type) : 
	fDetectorId(id),
	fType(type)
{
}

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
	else if (name == "eHodoscope")
		return Detector::eHodoscope;
	else if (name == "eSaltyWCD")
		return Detector::eSaltyWCD;
	else if (name == "eDummy")
		return Detector::eDummy;
	else {
		Logger::Print("Unknown detector type!", WARNING, "StringToType");
		return Detector::eUnknown;
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
Detector::SetDetectorProperties(const ptree &tree, DefaultProperties &defProp)
{
	/*	setting the default property values in the Detector:
			these values are load from the DetectorProperties.xml
			in Framework/ConfigManager/DefaultProperties.cc 
	*/
	SetTankRadius(defProp.gTankRadius);
	SetTankHeight(defProp.gTankHeight);
	SetTankThickness(defProp.gTankThickness);
	SetImpuritiesFraction(defProp.gImpuritiesFraction);

	SetNBars(defProp.gNumberOfBars);
	SetNPanels(defProp.gNumberOfPanels);
	SetBarWidth(defProp.gBarWidth);
	SetBarLength(defProp.gBarLength);
	SetBarThickness(defProp.gBarThickness);
	SetBarCoatingThickness(defProp.gCoatingThickness);
	SetRotationAngle(defProp.gRotationAngle);
	SetCasingThickness(defProp.gCasingThickness);
	SetFiberLength(defProp.gFiberLength);
	SetFiberRadius(defProp.gFiberRadius);
	SetCladdingThickness(defProp.gCladdingThickness);
	SetDistanceBtwPanels(defProp.gDistancePanels);

	SetLength(defProp.gLength);
	SetWidth(defProp.gWidth);
	SetThickness(defProp.gThickness);

	SetGroundSizeX(defProp.gGroundSizeX);
	SetGroundSizeY(defProp.gGroundSizeY);
	SetGroundSizeZ(defProp.gGroundSizeZ);

	/*
		Now that the default properties are set, look for a property
		in the DetectorList.xml and override in case of exist. 
	*/

	for (const auto& v : tree.get_child("")) {
		string xmlLabel = v.first;

		if (xmlLabel != "<xmlattr>") {
			string xmlValue = v.second.data();
			// loop over XML tags to set the corresponding value
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
			else if (xmlLabel == "impuritiesFraction") {
				double value = stod(xmlValue);
				SetImpuritiesFraction(value);
			}
			else if (xmlLabel == "numberOfBars") {
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
			else if (xmlLabel == "rotationAngle") {
				double value = stod(xmlValue);
				SetRotationAngle(value);
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
			else if (xmlLabel == "length") {
				double value = stod(xmlValue);
				double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
				SetLength(value * unit);
			}
			else if (xmlLabel == "width") {
				double value = stod(xmlValue);
				double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
				SetWidth(value * unit);
			}
			else if (xmlLabel == "thickness") {
				double value = stod(xmlValue);
				double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
				SetThickness(value * unit);
			}
			else if (xmlLabel == "distancePanels") {
				double value = stod(xmlValue);
				double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
				SetDistanceBtwPanels(value * unit);
			}
			else if (xmlLabel == "numberOfPanels") {
				int value = stoi(xmlValue);
				SetNPanels(value);
			}
			else if (xmlLabel == "groundSizeX") {
				double value = stod(xmlValue);
				double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
				SetGroundSizeX(value * unit);
			}
			else if (xmlLabel == "groundSizeY") {
				double value = stod(xmlValue);
				double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
				SetGroundSizeY(value * unit);
			}
			else if (xmlLabel == "groundSizeZ") {
				double value = stod(xmlValue);
				double unit = G4UnitDefinition::GetValueOf(v.second.get<string>("<xmlattr>.unit"));
				SetGroundSizeZ(value * unit);
			}
		}
	}
}

void
Detector::SetRotationAngle(const double angle)
{
	double angle_rad = angle * (M_PI / 180.);
	fRotationAngle = angle_rad;
}