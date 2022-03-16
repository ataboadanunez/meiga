// implementation of the G4MudulusDetectorConstruction class
#include "G4MudulusDetectorConstruction.h"
#include "Materials.h"

#include "Mudulus.h"

#include "G4UnitsTable.hh"
#include <iostream>

using namespace std;

G4MudulusDetectorConstruction::G4MudulusDetectorConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{ 
}

G4MudulusDetectorConstruction::~G4MudulusDetectorConstruction() 
	{ }


G4VPhysicalVolume*
G4MudulusDetectorConstruction::CreateDetector()
{

	CreateWorld();
  CreateGround();
	PlaceDetector(fEvent);
  /***
   The function PlaceDetector() should be a replacement of ReadModuleList in G4RockSimulator.
   It should call an specific detector class (type) from G4Models and place it at a given
   position in a given mother volume.

  ***/
	return physWorld;
}

void
G4MudulusDetectorConstruction::CreateWorld()
{


	solidWorld 	= new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld	 =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);

}

void
G4MudulusDetectorConstruction::CreateGround()
{
  solidGround = new G4Box("Ground", fGroundSizeX/2, fGroundSizeY/2, fGroundSizeZ/2);
  G4VisAttributes brown(G4Colour::Brown());
  logicGround = new G4LogicalVolume(solidGround, Materials().StdRock, "Ground");
  logicGround->SetVisAttributes(brown);
  physGround  =  new G4PVPlacement(nullptr, G4ThreeVector(), logicGround, "Ground", logicWorld, false, 0, fCheckOverlaps);
}


void
G4MudulusDetectorConstruction::PlaceDetector(Event& theEvent)
{
  
  // loop over Detectors Range
  for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {
    auto& currentDet = detIt->second;
    Mudulus::BuildDetector(logicWorld, currentDet, theEvent);
  }

}

G4VPhysicalVolume* 
G4MudulusDetectorConstruction::Construct() 
{

	if (!physWorld) {
		//CreateElements();
		return CreateDetector();
	}
	return physWorld;

}
