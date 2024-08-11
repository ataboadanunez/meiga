// Meiga headers
#include "Scintillator.h"
#include "Geometry.h"
#include "G4MDetectorAction.h"
#include "ConfigManager.h"

// Geant4 headers
#include "G4SDManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
// C++
#include <iostream>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;
using boost::property_tree::ptree;


Scintillator::Scintillator(const int id, const Detector::DetectorType type) :
	Detector(id, type)
{
	fName = "Scintillator";
	fOverwrittenPropertiesVector.clear();
	SetDefaultProperties();
}

void Scintillator::BuildDetector(G4LogicalVolume *logMother, Event &aEvent, G4bool fCheckOverlaps)
{

	int detectorId = fDetectorId; //detector.GetId();
	G4ThreeVector  detectorPos = Geometry::ToG4Vector(GetDetectorPosition(), 1.);

	ostringstream namedetector;
	namedetector.str("");
	namedetector << "Scintillator";
	ostringstream msg; 
	msg << "Building detector: " << namedetector.str() << ", ID: " << detectorId;
	msg << "Detector position: " << "(" << detectorPos.getX() / CLHEP::cm << ", " << detectorPos.getY() << ", " << detectorPos.getZ() << ")";
	Logger::Print(msg, INFO, "BuildDetector");

	// solids
	G4Box* solidCoating = nullptr;
	G4Box* solidScinBar = nullptr;

	// logical volumes
	G4LogicalVolume* logicCoating = nullptr;
	G4LogicalVolume* logicScinBar = nullptr;

	// following variables are read from Detector class
	G4double fCoatingThickness = GetBarCoatingThickness();
	G4double fBarWidth = GetBarWidth();
	G4double fBarLength = GetBarLength();
	G4double fBarThickness = GetBarThickness();
	G4int fNBars = GetNBars();
	G4double fHalfWidth = 0.5*fBarWidth*fNBars; 

	auto sdMan = G4SDManager::GetSDMpointer();
	
	// Bars: Coating + Scintillator bar
	solidCoating  	= new G4Box("BarCoating", 0.5*fBarLength + fCoatingThickness, 0.5*fBarWidth + fCoatingThickness, 0.5*fBarThickness + fCoatingThickness);
	solidScinBar   	= new G4Box("BarScin", 0.5*fBarLength, 0.5*fBarWidth, 0.5*fBarThickness);
	// register Bar coating as a LogicalSkinSurface
	new G4LogicalSkinSurface("BarCoating", logicCoating, Materials().ScinOptSurf);

	// logical volumes
	logicCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, "BarCoating", 0, 0, 0);
	logicScinBar = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, "BarScin", 0, 0, 0);

	// bars placement
	for (G4int i=0; i<fNBars; ++i) {

		// definitions for bar placement
		int barId = i+1;

		// naming
		G4String nameCoating = "BarCoating_"+to_string(barId);
		G4String nameScinBar = "BarScin_"+to_string(barId);
		
		// place bars along the y-axis
		G4double yPos = detectorPos.getY() + i*(fBarWidth + 2*fCoatingThickness);
		// shift each bar position by 1/2 of the panel width to center the detector
		yPos -= fHalfWidth;

		

		// physical volumes
		new G4PVPlacement(nullptr, G4ThreeVector(detectorPos.getX(), yPos, detectorPos.getZ()), logicCoating, 
			nameCoating, logMother, false, barId, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(), logicScinBar, 
			nameScinBar, logicCoating, false, barId, fCheckOverlaps);

  }

  	// register scintillator bar as sensitive detector
	G4MDetectorAction* const scinSD = new G4MDetectorAction("BarScin", detectorId, aEvent);
	sdMan->AddNewDetector(scinSD);
	logicScinBar->SetSensitiveDetector(scinSD);
}

void Scintillator::SetDefaultProperties()
{
	if (fDefaultPropertiesFile.empty()) 
		return;

	Logger::Print("Setting default properties from file "+fDefaultPropertiesFile, INFO, "SetDefaultProperties");
	ptree tree;
	read_xml(fDefaultPropertiesFile, tree);
	string branchName = "detectorProperties";

	SetNBars( ConfigManager::GetPropertyFromXML<int>(tree, branchName, "numberOfBars", false) );
	SetNPanels( ConfigManager::GetPropertyFromXML<int>(tree, branchName, "numberOfPanels", false) );
	SetBarWidth( ConfigManager::GetPropertyFromXML<double>(tree, branchName, "barWidth") );
	SetBarLength( ConfigManager::GetPropertyFromXML<double>(tree, branchName, "barLength") );
	SetBarThickness( ConfigManager::GetPropertyFromXML<double>(tree, branchName, "barThickness") );
	SetBarCoatingThickness( ConfigManager::GetPropertyFromXML<double>(tree, branchName, "coatingThickness") );
	SetCasingThickness( ConfigManager::GetPropertyFromXML<double>(tree, branchName, "casingThickness") );
	SetRotationAngle( ConfigManager::GetPropertyFromXML<double>(tree, branchName, "rotationAngle", false) );
	SetFiberLength( ConfigManager::GetPropertyFromXML<double>(tree, branchName, "fiberLength") );	
	SetFiberRadius( ConfigManager::GetPropertyFromXML<double>(tree, branchName, "fiberRadius") );
	SetCladdingThickness( ConfigManager::GetPropertyFromXML<double>(tree, branchName, "claddingThickness") );
	SetDistanceBtwPanels( ConfigManager::GetPropertyFromXML<double>(tree, branchName, "distancePanels") );

}

void Scintillator::SetDetectorProperties(const boost::property_tree::ptree &aTree)
{
	vector<double> detPosition;
	detPosition.reserve(3);

	for (const auto& property : aTree) {
		string xmlLabel = property.first;
		string xmlValue = property.second.data();

		if(xmlLabel == "<xmlattr>")
			continue;
		if(xmlLabel == "<xmlcomment>")
			continue;

		if ( xmlLabel == "x" || xmlLabel == "y" || xmlLabel == "z") {
			string value = property.second.data();
			boost::algorithm::trim(value);
			double dValue = stod(value);
			string unit = property.second.get<string>("<xmlattr>.unit");
			double coord = G4UnitDefinition::GetValueOf(unit) * dValue;
			detPosition.push_back(coord);
		} else if (xmlLabel == "numberOfBars") {
			int value = stoi(xmlValue);
			fOverwrittenPropertiesVector.push_back(xmlLabel);
			SetNBars(value);
		}
		else if (xmlLabel == "barLength") {
			double value = stod(xmlValue);
			double unit = G4UnitDefinition::GetValueOf(property.second.get<string>("<xmlattr>.unit"));
			fOverwrittenPropertiesVector.push_back(xmlLabel);
			SetBarLength(value * unit);
		}
		else if (xmlLabel == "barWidth") {
			double value = stod(xmlValue);
			double unit = G4UnitDefinition::GetValueOf(property.second.get<string>("<xmlattr>.unit"));
			fOverwrittenPropertiesVector.push_back(xmlLabel);
			SetBarWidth(value * unit);
		}
		else if (xmlLabel == "barThickness") {
			double value = stod(xmlValue);
			double unit = G4UnitDefinition::GetValueOf(property.second.get<string>("<xmlattr>.unit"));
			fOverwrittenPropertiesVector.push_back(xmlLabel);
			SetBarThickness(value * unit);
		}
		else if (xmlLabel == "coatingThickness") {
			double value = stod(xmlValue);
			double unit = G4UnitDefinition::GetValueOf(property.second.get<string>("<xmlattr>.unit"));
			fOverwrittenPropertiesVector.push_back(xmlLabel);
			SetBarCoatingThickness(value * unit);
		}
		else if (xmlLabel == "casingThickness") {
			double value = stod(xmlValue);
			double unit = G4UnitDefinition::GetValueOf(property.second.get<string>("<xmlattr>.unit"));
			fOverwrittenPropertiesVector.push_back(xmlLabel);
			SetCasingThickness(value * unit);
		}
		else if (xmlLabel == "rotationAngle") {
			double value = stod(xmlValue);
			SetRotationAngle(value);
		}
		else if (xmlLabel == "fiberLength") {
			double value = stod(xmlValue);
			double unit = G4UnitDefinition::GetValueOf(property.second.get<string>("<xmlattr>.unit"));
			fOverwrittenPropertiesVector.push_back(xmlLabel);
			SetFiberLength(value * unit);
		}
		else if (xmlLabel == "fiberRadius") {
			double value = stod(xmlValue);
			double unit = G4UnitDefinition::GetValueOf(property.second.get<string>("<xmlattr>.unit"));
			fOverwrittenPropertiesVector.push_back(xmlLabel);
			SetFiberRadius(value * unit);
		}
		else if (xmlLabel == "claddingThickness") {
			double value = stod(xmlValue);
			double unit = G4UnitDefinition::GetValueOf(property.second.get<string>("<xmlattr>.unit"));
			fOverwrittenPropertiesVector.push_back(xmlLabel);
			SetCladdingThickness(value * unit);
		}
		else if (xmlLabel == "distancePanels") {
			double value = stod(xmlValue);
			double unit = G4UnitDefinition::GetValueOf(property.second.get<string>("<xmlattr>.unit"));
			fOverwrittenPropertiesVector.push_back(xmlLabel);
			SetDistanceBtwPanels(value * unit);
		}
		else if (xmlLabel == "numberOfPanels") {
			int value = stoi(xmlValue);
			fOverwrittenPropertiesVector.push_back(xmlLabel);
			SetNPanels(value);
		}
		else {
			ostringstream msg;
			msg << xmlLabel << " is not a property of detector " << TypeToString(fType) << ". Skipping...";
			Logger::Print(msg, WARNING, "SetDetectorProperties");
		}
	}
	SetDetectorPosition(detPosition);
	Logger::PrintVector(fDetectorPosition, "Detector position: ", INFO, "SetDetectorProperties");
	
	if(!fOverwrittenPropertiesVector.empty()) {
		Logger::PrintVector(fOverwrittenPropertiesVector, "Overwritten properties for detector " + TypeToString(fType), INFO, "SetDetectorProperties");
	}
}

void Scintillator::SetRotationAngle(double angle)
{
	double angle_rad = angle * (M_PI / 180.);
	fRotationAngle = angle_rad;
}
