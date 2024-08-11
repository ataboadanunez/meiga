#include "Detector.h"
#include "Logger.h"
#include "Utilities.h"
#include "ConfigManager.h"
#include "ConfigFilePath.h"

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
	fOverwrittenPropertiesVector.clear();
	fDefaultPropertiesFile = Utilities::ConcatenatePaths(GetConfigFilePath(), "DetectorProperties.xml");
	SetDefaultProperties();
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
		Logger::Print("Unknown detector name: "+name, WARNING, "StringToType");
		return Detector::eUnknown;
	}
}

std::string Detector::TypeToString(const DetectorType aType)
{
	string name = string();
    switch (aType)
	{
	case Detector::eWCD:
		name = "eWCD";
		break;
	case Detector::eSaltyWCD:
		name = "eSaltyWCD";
		break;
	case Detector::eScintillator:
		name = "eScintillator";
		break;
	case Detector::eMusaic:
		name = "eMusaic";
		break;
	case Detector::eMudulus:
		name = "eMudulus";
		break;
	case Detector::eHodoscope:
		name = "eHodoscope";
		break;
	case Detector::eDummy:
		name = "eDummy";
		break;
	default:
		Logger::Print("Unknown detector type: "+aType, WARNING, "TypeToString");
		break;
	}
	return name;
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
Detector::SetDetectorProperties(const ptree &aTree)
{
}

void Detector::SetDefaultProperties()
{
}