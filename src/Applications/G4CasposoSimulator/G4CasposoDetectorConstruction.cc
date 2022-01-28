// implementation of the G4CasposoDetectorConstruction class
#include <iostream>

#include "G4CasposoDetectorConstruction.h"
// include detector models (should be in separate header)
#include "Materials.h"
#include "Musaic.h"
#include "Mudulus.h"

#include "G4UnitsTable.hh"

using namespace std;


G4CasposoDetectorConstruction::G4CasposoDetectorConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{ 
}

G4CasposoDetectorConstruction::~G4CasposoDetectorConstruction() 
	{ }

G4VPhysicalVolume*
G4CasposoDetectorConstruction::CreateDetector()
{

	CreateWorld();
	CreateGround();
	PlaceDetector(fEvent);
	return physWorld;
}

void
G4CasposoDetectorConstruction::CreateWorld()
{


	solidWorld  = new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld  =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);

}

void
G4CasposoDetectorConstruction::CreateGround()
{
	solidGround = new G4Box("Ground", fGroundSizeX/2, fGroundSizeY/2, fGroundSizeZ/2);
	G4VisAttributes brown(G4Colour::Brown());
	logicGround = new G4LogicalVolume(solidGround, Materials().StdRock, "Ground");
	logicGround->SetVisAttributes(brown);
	physGround  =  new G4PVPlacement(nullptr, G4ThreeVector(), logicGround, "Ground", logicWorld, false, 0, fCheckOverlaps);
}


void
G4CasposoDetectorConstruction::PlaceDetector(Event& theEvent)
{
	
	//Detector& detector = theEvent.GetDetector();
	int nDetectors = theEvent.GetNDetectors();
	// loop in Detectors vector
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {
		auto& currentDet = detIt->second;
		Detector::DetectorType detType = currentDet.GetType();
		
		Mudulus().BuildDetector(logicGround, currentDet, theEvent);
	}

}

G4VPhysicalVolume* 
G4CasposoDetectorConstruction::Construct() 
{

	if (!physWorld) {
		//CreateElements();
		return CreateDetector();
	}
	return physWorld;

}

// void
// G4CasposoDetectorConstruction::CreateElements() 
// {
//   // example function to define elements and materials
	
//   G4NistManager *nistManager = G4NistManager::Instance();
	
//   elN  = new G4Element("Nitrogen", "N", 7, 14.01 * g/mole);
//   elO  = new G4Element("Oxygen", "O", 8, 16.00 * g/mole);
//   elH  = new G4Element("Hydrogen", "H", 1, 1.01 * g/mole);
//   elSi = new G4Element("Silicon", "Si", 14, 28.09 * g/mole);
	
//   Air = new G4Material("Air", 1.29e-3 * g/cm3, 2);
//   Air->AddElement(elN, 70.0 * perCent); //percent of material, ex:70% Nitrogen
//   Air->AddElement(elO, 30.0 * perCent); 
	
//   // quartz (SiO2)
//   Quartz = new G4Material("Quartz", 2.65 * g/cm3, 2);
//   Quartz->AddElement(elSi, 1);
//   Quartz->AddElement(elO, 2);

//   Water = nistManager->FindOrBuildMaterial("G4_WATER");

// }

// G4VPhysicalVolume*
// G4CasposoDetectorConstruction::CreateDetector()
// {

// 	CreateWorld();
// 	CreateHall();
// 	//PlaceDetector();
//   /***
//    The function PlaceDetector() should be a replacement of ReadModuleList in G4CasposoSimulator.
//    It should call an specific detector class (type) from Framework/Detector/ and place it at a given
//    position in a given mother volume.

//   ***/
// 	return physWorld;
// }

// void
// G4CasposoDetectorConstruction::CreateWorld()
// {

//   solidWorld 	= new G4Box("World", fWorldSizeX, fWorldSizeY, fWorldSizeZ);
//   logicWorld = new G4LogicalVolume(solidWorld, Quartz, "World");
//   physWorld	 =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);
	
// }

// void
// G4CasposoDetectorConstruction::CreateHall()
// {
//         // roca y aire (eventualmente Rock sera "ground"... y el hall sera un hall de verdad... 1:35AM)
//         solidRock       = new G4Box("Quartz", fRockSizeX, fRockSizeY, fRockSizeZ);
//         solidHall       = new G4Box("Hall", fHallSizeX, fHallSizeY, fHallSizeZ);
//         G4VisAttributes brown(G4Colour::Brown());
// 	G4VisAttributes magenta(G4Colour::Magenta());
//         logicRock = new G4LogicalVolume(solidRock, Quartz, "Rock");
//         logicRock->SetVisAttributes(brown);
//         physRock        =  new G4PVPlacement(nullptr, G4ThreeVector(), logicRock, "Rock", logicWorld, false, 0, fCheckOverlaps);
//         logicHall = new G4LogicalVolume(solidHall, Air, "Hall");
// 	logicHall->SetVisAttributes(magenta);
//         physHall        =  new G4PVPlacement(nullptr, G4ThreeVector(fHallPosX, fHallPosY, fHallPosZ), logicHall, "Hall", logicRock, false, 0, fCheckOverlaps);
// }

// G4VPhysicalVolume* 
// G4CasposoDetectorConstruction::Construct() 
// {

// 	//SetDetectorParameters();

// 	if (!physWorld) {
// 		CreateElements();
// 		return CreateDetector();
// 	}
// 	return physWorld;

// }

