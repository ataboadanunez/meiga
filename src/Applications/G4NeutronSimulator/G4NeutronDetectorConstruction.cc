// implementation of the G4NeutronDetectorConstruction class
#include <iostream>

#include "G4NeutronDetectorConstruction.h"
// include Materials from G4Models
#include "Materials.h"

#include "G4UnitsTable.hh"

using namespace std;

G4NeutronDetectorConstruction::G4NeutronDetectorConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{
	fGroundSizeX = fEvent.GetDetector(0).GetGroundSizeX();
	fGroundSizeY = fEvent.GetDetector(0).GetGroundSizeY();
	fGroundSizeZ = fEvent.GetDetector(0).GetGroundSizeZ();

	fWorldSizeX = fGroundSizeX + 10*m;
	fWorldSizeY = fGroundSizeY + 10*m;
	fWorldSizeZ = fGroundSizeZ + 10*m;
}

G4NeutronDetectorConstruction::~G4NeutronDetectorConstruction() 
	{ }


G4VPhysicalVolume*
G4NeutronDetectorConstruction::CreateDetector()
{

	CreateWorld();
	CreateGround();
	PlaceDetector(fEvent);
	return physWorld;
}

void
G4NeutronDetectorConstruction::CreateWorld()
{


	solidWorld 	= new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld	 =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);

}

void
G4NeutronDetectorConstruction::CreateGround()
{
	
	std::cout << "[INFO] G4VUserDetectorConstruction::CreateGround: Ground Size = (" << fGroundSizeX / CLHEP::m << ", " << fGroundSizeY / CLHEP::m << ", " << fGroundSizeZ / CLHEP::m << ") m" << std::endl;

	solidGround = new G4Box("Ground", fGroundSizeX/2, fGroundSizeY/2, fGroundSizeZ/2);
	G4VisAttributes brown(G4Colour::Brown());
	logicGround = new G4LogicalVolume(solidGround, Materials().StdRock, "Ground");
	logicGround->SetVisAttributes(brown);
	// physGround  =  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, -fWorldSizeZ/2 + fGroundSizeZ/2), logicGround, "Ground", logicWorld, false, 0, fCheckOverlaps);
	physGround  =  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logicGround, "Ground", logicWorld, false, 0, fCheckOverlaps);
}


void
G4NeutronDetectorConstruction::PlaceDetector(Event& theEvent)
{
	
	// loop over Detectors Range
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {
		auto& currentDet = detIt->second;
		BuildDetector(logicWorld, currentDet, theEvent, true);

	}

}

G4VPhysicalVolume* 
G4NeutronDetectorConstruction::Construct() 
{

	if (!physWorld) {
		//CreateElements();
		return CreateDetector();
	}
	return physWorld;

}
