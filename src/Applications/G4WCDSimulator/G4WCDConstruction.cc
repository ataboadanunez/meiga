// implementation of the G4WCDConstruction class
#include <iostream>

#include "G4WCDConstruction.h"
// include detector models (should be in separate header)
#include "Materials.h"
#include "Musaic.h"
#include "Mudulus.h"
//#include "RCDetector.h"

#include "G4UnitsTable.hh"

using namespace std;

G4WCDConstruction::G4WCDConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{ 
}

G4WCDConstruction::~G4WCDConstruction() 
	{ }


G4VPhysicalVolume*
G4WCDConstruction::CreateDetector()
{

	CreateWorld();
	//CreateGround();
	PlaceDetector(fEvent);
	return physWorld;
}

void
G4WCDConstruction::CreateWorld()
{


	solidWorld 	= new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld	 =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);

}

// void
// G4WCDConstruction::CreateGround()
// {
// 	solidGround = new G4Box("Ground", fGroundSizeX/2, fGroundSizeY/2, fGroundSizeZ/2);
// 	G4VisAttributes brown(G4Colour::Brown());
// 	logicGround = new G4LogicalVolume(solidGround, Materials().StdRock, "Ground");
// 	logicGround->SetVisAttributes(brown);
// 	physGround  =  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, -fWorldSizeZ/2 + fGroundSizeZ/2), logicGround, "Ground", logicWorld, false, 0, fCheckOverlaps);
// }


void
G4WCDConstruction::PlaceDetector(Event& theEvent)
{
	
	//Detector& detector = theEvent.GetDetector();
	int nDetectors = theEvent.GetNDetectors();
	// loop in detector vector
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {
		auto& currentDet = detIt->second;
		cout << "[DEBUG] G4WCDConstruction: Detector Type = " << currentDet.GetType() << endl;
			BuildDetector(logicWorld, currentDet, theEvent, true);

	}

}

G4VPhysicalVolume* 
G4WCDConstruction::Construct() 
{

	if (!physWorld) {
		//CreateElements();
		return CreateDetector();
	}
	return physWorld;

}
