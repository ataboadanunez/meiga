// implementation of the G4ExDetectorConstruction class
#include <iostream>

#include "G4ExDetectorConstruction.h"
// include Materials from G4Models
#include "Materials.h"

#include "G4UnitsTable.hh"

using namespace std;

G4ExDetectorConstruction::G4ExDetectorConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{
	cout << "...G4ExDetectorConstruction..." << endl;
	fCheckOverlaps = theEvent.GetConfig().fCheckOverlaps;
}

G4ExDetectorConstruction::~G4ExDetectorConstruction() 
{ 
	delete solidWorld;
	delete logicWorld;
	delete physWorld;
	delete solidGround;
	delete logicGround;
	delete physGround;
}


G4VPhysicalVolume*
G4ExDetectorConstruction::CreateDetector()
{

	CreateWorld();
	// CreateGround();
	PlaceDetector(fEvent);
	return physWorld;
}

void
G4ExDetectorConstruction::CreateWorld()
{


	solidWorld 	= new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld	 =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);

}

void
G4ExDetectorConstruction::CreateGround()
{
	solidGround = new G4Box("Ground", fGroundSizeX/2, fGroundSizeY/2, fGroundSizeZ/2);
	G4VisAttributes brown(G4Colour::Brown());
	logicGround = new G4LogicalVolume(solidGround, Materials().StdRock, "Ground");
	logicGround->SetVisAttributes(brown);
	physGround  =  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, -fWorldSizeZ/2 + fGroundSizeZ/2), logicGround, "Ground", logicWorld, false, 0, fCheckOverlaps);
}


void
G4ExDetectorConstruction::PlaceDetector(Event& theEvent)
{
	
	// loop in Detectors Range
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {
		auto& currentDet = detIt->second;
		BuildDetector(logicWorld, currentDet, theEvent, fCheckOverlaps);

	}

}

G4VPhysicalVolume* 
G4ExDetectorConstruction::Construct() 
{

	if (!physWorld) {
		//CreateElements();
		return CreateDetector();
	}
	return physWorld;

}
