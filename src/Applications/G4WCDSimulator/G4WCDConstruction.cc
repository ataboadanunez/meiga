// implementation of the G4WCDConstruction class
#include <iostream>

#include "G4WCDConstruction.h"
#include "Materials.h"

#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

using namespace std;

G4WCDConstruction::G4WCDConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{ 
	fCheckOverlaps = fEvent.GetConfig().fCheckOverlaps;
}

G4WCDConstruction::~G4WCDConstruction() 
	{ }


G4VPhysicalVolume*
G4WCDConstruction::CreateDetector()
{

	CreateWorld();
	PlaceDetector(fEvent);
	return physWorld;
}

void
G4WCDConstruction::CreateWorld()
{


	solidWorld 	= new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);

}

void
G4WCDConstruction::CreateGround()
{
	solidGround = new G4Box("Ground", fGroundSizeX/2, fGroundSizeY/2, fGroundSizeZ/2);
	G4VisAttributes brown(G4Colour::Brown());
	logicGround = new G4LogicalVolume(solidGround, Materials().StdRock, "Ground");
	logicGround->SetVisAttributes(brown);
	physGround  =  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, -fWorldSizeZ/2 + fGroundSizeZ/2), logicGround, "Ground", logicWorld, false, 0, fCheckOverlaps);
}


void
G4WCDConstruction::PlaceDetector(Event& aEvent)
{
	for (auto &pair : aEvent.DetectorRange()) {
		Detector& currentDet = *(pair.second);
		currentDet.BuildDetector(logicWorld, aEvent, fCheckOverlaps);
	}
}


G4VPhysicalVolume* 
G4WCDConstruction::Construct() 
{
	if (!physWorld) {
		return CreateDetector();
	}
	return physWorld;
}

void G4WCDConstruction::ConstructSDandField()
{	
	// for (auto detIt = fEvent.DetectorRange().begin(); detIt != fEvent.DetectorRange().end(); detIt++) {
	// 	Detector &currentDet = detIt->second;
	// 	ConstructSenstiveDetector(currentDet, fEvent);
	// }
}
