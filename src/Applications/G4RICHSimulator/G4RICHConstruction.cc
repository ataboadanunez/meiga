// implementation of the G4RICHConstruction class
#include <iostream>

#include "G4RICHConstruction.h"
#include "Materials.h"

#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

using namespace std;

G4RICHConstruction::G4RICHConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{ 
}

G4RICHConstruction::~G4RICHConstruction() 
	{ }


G4VPhysicalVolume*
G4RICHConstruction::CreateDetector()
{

	CreateWorld();
	//CreateGround();
	PlaceDetector(fEvent);
	return physWorld;
}

void
G4RICHConstruction::CreateWorld()
{


	solidWorld 	= new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld	 =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);

}

void
G4RICHConstruction::CreateGround()
{
	solidGround = new G4Box("Ground", fGroundSizeX/2, fGroundSizeY/2, fGroundSizeZ/2);
	G4VisAttributes brown(G4Colour::Brown());
	logicGround = new G4LogicalVolume(solidGround, Materials().StdRock, "Ground");
	logicGround->SetVisAttributes(brown);
	physGround  =  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, -fWorldSizeZ/2 + fGroundSizeZ/2), logicGround, "Ground", logicWorld, false, 0, fCheckOverlaps);
}


void
G4RICHConstruction::PlaceDetector(Event& theEvent)
{
	
	// loop in detector vector
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {
		auto& currentDet = detIt->second;
		BuildDetector(logicWorld, currentDet, theEvent, true);

	}

}


G4VPhysicalVolume* 
G4RICHConstruction::Construct() 
{

	if (!physWorld) {
		return CreateDetector();
	}
	return physWorld;

}
