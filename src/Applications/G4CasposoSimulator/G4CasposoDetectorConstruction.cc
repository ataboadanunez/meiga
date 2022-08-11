// implementation of the G4CasposoDetectorConstruction class
#include <iostream>

#include "G4CasposoDetectorConstruction.h"
#include "Materials.h"

#include "G4SDManager.hh"
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
	
	auto& simData = fEvent.GetSimData();
	simData.SetGroundLength(fGroundSizeX);
	simData.SetGroundWidth(fGroundSizeY);
	std::cout << "[DEBUG] G4CasposoDetectorConstruction::CreateGround: GroundThickness = " << fGroundSizeZ /CLHEP::m << std::endl;
	simData.SetGroundThickness(fGroundSizeZ);
	std::cout << "[DEBUG] G4CasposoDetectorConstruction::SimData: GroundThickness = " << simData.GetGroundThickness() / CLHEP::m << std::endl;
	
	solidGround = new G4Box("Ground", fGroundSizeX/2, fGroundSizeY/2, fGroundSizeZ/2);
	G4VisAttributes brown(G4Colour::Brown());
	logicGround = new G4LogicalVolume(solidGround, Materials().StdRock, "Ground");
	logicGround->SetVisAttributes(brown);
	//physGround  =  new G4PVPlacement(nullptr, G4ThreeVector(), logicGround, "Ground", logicWorld, false, 0, fCheckOverlaps);
}


void
G4CasposoDetectorConstruction::PlaceDetector(Event& theEvent)
{
	
	// loop in detector vector
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {
		auto& currentDet = detIt->second;
		BuildDetector(logicWorld, currentDet, theEvent, true);
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
