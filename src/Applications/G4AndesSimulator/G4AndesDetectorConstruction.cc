// implementation of the G4AndesDetectorConstruction class
#include <iostream>

#include "G4AndesDetectorConstruction.h"
#include "Materials.h"

#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

using namespace std;


G4AndesDetectorConstruction::G4AndesDetectorConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{ 
}

G4AndesDetectorConstruction::~G4AndesDetectorConstruction() 
	{ }

G4VPhysicalVolume*
G4AndesDetectorConstruction::CreateDetector()
{

	CreateWorld();
	CreateGround();
	PlaceDetector(fEvent);
	return physWorld;
}

void
G4AndesDetectorConstruction::CreateWorld()
{


	solidWorld  = new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld  =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);

}

void
G4AndesDetectorConstruction::CreateGround()
{
	
	auto& simData = fEvent.GetSimData();
	simData.SetGroundLength(fGroundSizeX);
	simData.SetGroundWidth(fGroundSizeY);
	std::cout << "[DEBUG] G4AndesDetectorConstruction::CreateGround: GroundThickness = " << fGroundSizeZ /CLHEP::m << std::endl;
	simData.SetGroundThickness(fGroundSizeZ);
	std::cout << "[DEBUG] G4AndesDetectorConstruction::SimData: GroundThickness = " << simData.GetGroundThickness() / CLHEP::m << std::endl;
	
	solidGround = new G4Box("Ground", fGroundSizeX/2, fGroundSizeY/2, fGroundSizeZ/2);
	G4VisAttributes brown(G4Colour::Brown());
	logicGround = new G4LogicalVolume(solidGround, Materials().StdRock, "Ground");
	logicGround->SetVisAttributes(brown);
	new G4PVPlacement(nullptr, G4ThreeVector(), logicGround, "Ground", logicWorld, false, 0, fCheckOverlaps);


}


void
G4AndesDetectorConstruction::PlaceDetector(Event& theEvent)
{
	
	const Event::Config &cfg = theEvent.GetConfig();
	// loop in detector vector
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {
		auto& currentDet = detIt->second;
		BuildDetector(logicWorld, currentDet, theEvent, cfg.fCheckOverlaps);
	}

}

G4VPhysicalVolume* 
G4AndesDetectorConstruction::Construct() 
{

	if (!physWorld) {
		//CreateElements();
		return CreateDetector();
	}
	return physWorld;

}
