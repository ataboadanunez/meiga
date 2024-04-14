// implementation of the G4LeadDetectorConstruction class
#include <iostream>

#include "G4LeadDetectorConstruction.h"
#include "Materials.h"

#include <G4SDManager.hh>
#include <G4UnitsTable.hh>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using namespace std;


G4LeadDetectorConstruction::G4LeadDetectorConstruction(Event& aEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(aEvent)
{ 
	
	Event::Config &cfg = aEvent.GetConfig();
	string aConfigFileName = cfg.fConfigurationFileName;
	cout << "[INFO] G4LeadDetectorConstruction::G4LeadDetectorConstruction: Reading Lead information from Configuration File " << aConfigFileName << endl;
	ptree tree;
	read_json(aConfigFileName, tree);
	
	fSimulateBrick = tree.get<bool>("LeadBrick.Simulate", false);
	if (tree.get_child_optional("LeadBrick.Position")) {
		vector<double> brickPosition;
		for (const auto &item : tree.get_child("LeadBrick.Position")) {
			brickPosition.push_back(item.second.get_value<double>());
		}
		// brick position coordinates in cm
		fBrickPosX = brickPosition[0] * CLHEP::cm;
		fBrickPosY = brickPosition[1] * CLHEP::cm;
		fBrickPosZ = brickPosition[2] * CLHEP::cm;
	}
	if (tree.get_child_optional("LeadBrick.Size")) {
		vector<double> brickSize;
		for (const auto &item : tree.get_child("LeadBrick.Size")) {
			brickSize.push_back(item.second.get_value<double>());
		}
		// brick size in cm
		fBrickSizeX = brickSize[0] * CLHEP::cm;
		fBrickSizeY = brickSize[1] * CLHEP::cm;
		fBrickSizeZ = brickSize[2] * CLHEP::cm;
	}
}	

G4LeadDetectorConstruction::~G4LeadDetectorConstruction() 
	{ }

G4VPhysicalVolume*
G4LeadDetectorConstruction::CreateDetector()
{

	CreateWorld();
	//CreateGround();
	PlaceDetector(fEvent);
	return physWorld;
}

void
G4LeadDetectorConstruction::CreateWorld()
{
	const Event::Config &cfg = fEvent.GetConfig();	
	// world size definitions
	fWorldSizeX = 5 * CLHEP::m;
	fWorldSizeY = 5 * CLHEP::m;
	fWorldSizeZ = 5 * CLHEP::m;

	solidWorld  = new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld  =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);
	if (fSimulateBrick) {
		cout << "[INFO] G4LeadDetectorConstruction::CreateWorld: Simulating Lead Brick with the following properties:" << endl;
		cout << "Size: [" << fBrickSizeX / CLHEP::cm << ", " << fBrickSizeY / CLHEP::cm << ", " << fBrickSizeZ / CLHEP::cm << "] cm" << endl;
		cout << "Position: [" << fBrickPosX / CLHEP::cm << ", " << fBrickPosY / CLHEP::cm << ", " << fBrickPosZ / CLHEP::cm << "] cm" << endl;
		solidBrick = new G4Box("LeadBrick", fBrickSizeX/2, fBrickSizeY/2, fBrickSizeZ/2);
		G4VisAttributes cgray(G4Colour::Gray());
		logicBrick = new G4LogicalVolume(solidBrick, Materials().Lead, "LeadBrick");
		logicBrick->SetVisAttributes(cgray);
		physBrick = new G4PVPlacement(nullptr, G4ThreeVector(fBrickPosX, fBrickPosY, fBrickPosZ), logicBrick, "LeadBrick", logicWorld, false, 0, cfg.fCheckOverlaps);
	}
}


void
G4LeadDetectorConstruction::PlaceDetector(Event& aEvent)
{
	
	const Event::Config &cfg = aEvent.GetConfig();
	// loop in detector vector
	for (auto detIt = aEvent.DetectorRange().begin(); detIt != aEvent.DetectorRange().end(); detIt++) {
		auto& currentDet = detIt->second;
		BuildDetector(logicWorld, currentDet, aEvent, cfg.fCheckOverlaps);
	}

}

G4VPhysicalVolume* 
G4LeadDetectorConstruction::Construct() 
{

	if (!physWorld) {
		//CreateElements();
		return CreateDetector();
	}
	return physWorld;

}
