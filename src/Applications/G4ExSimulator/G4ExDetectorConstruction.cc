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
G4ExDetectorConstruction::PlaceDetector(Event& aEvent)
{
	
	// loop in Detectors Range
	for (auto & detPair : aEvent.DetectorRange()) {
		auto& currentDet = *(detPair.second);
		// BuildDetector(logicWorld, currentDet, theEvent, fCheckOverlaps);
		currentDet.BuildDetector(logicWorld, aEvent, fCheckOverlaps);

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
