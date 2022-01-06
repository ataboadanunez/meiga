// implementation of the G4ExDetectorConstruction class
#include "G4ExDetectorConstruction.h"
#include "Materials.h"

#include "Musaic.h"
#include "Mudulus.h"

#include "G4UnitsTable.hh"
#include <iostream>

using namespace std;

G4ExDetectorConstruction::G4ExDetectorConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{ 
}

G4ExDetectorConstruction::~G4ExDetectorConstruction() 
	{ }


G4VPhysicalVolume*
G4ExDetectorConstruction::CreateDetector()
{

	CreateWorld();
	CreateGround();
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
	physGround  =  new G4PVPlacement(nullptr, G4ThreeVector(), logicGround, "Ground", logicWorld, false, 0, fCheckOverlaps);
}


void
G4ExDetectorConstruction::PlaceDetector(Event& theEvent)
{
	
	//Detector& detector = theEvent.GetDetector();
	int nDetectors = theEvent.GetNDetectors();
	// loop in Detectors Range
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {
		auto& currentDet = detIt->second;
		Detector::DetectorType detType = currentDet.GetType();
		std::cout << "[DEBUG] G4ExDetectorConstruction: Detector Type = " << currentDet.GetType() << std::endl;

		if (detType == Detector::DetectorType::eMusaic)
			Musaic().BuildDetector(logicGround, currentDet, theEvent);
		else if (detType == Detector::DetectorType::eMudulus)
			Mudulus().BuildDetector(logicGround, currentDet, theEvent);
		else
			std::cout << "[WARNING] Unknown detector type!" << std::endl;
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