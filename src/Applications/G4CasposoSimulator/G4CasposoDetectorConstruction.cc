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

	// size definitions
	G4double fWorldSizeX = 250 * CLHEP::m;
	G4double fWorldSizeY = 250 * CLHEP::m;
	G4double fWorldSizeZ = 200 * CLHEP::m;

	solidWorld  = new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld  =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);

}

void
G4CasposoDetectorConstruction::CreateGround()
{
	
	G4double fGroundSizeX = 230 * CLHEP::m;
	G4double fGroundSizeY = 230 * CLHEP::m;
	G4double fGroundSizeZ = 1 * CLHEP::m;

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
	physGround  =  new G4PVPlacement(nullptr, G4ThreeVector(), logicGround, "Ground", logicWorld, false, 0, fCheckOverlaps);

	// TANDAR
	G4double fDetectorLocationHeight = 6 *CLHEP::m;
	G4double fTandarHeight = 73 *CLHEP::m;
	G4double fTandarRadius = 5.9 * CLHEP::m;
	G4double fTandarThickness = 0.5 * CLHEP::m;

	G4double fTandarX = 0 * CLHEP::m;
	G4double fTandarY = 100 * CLHEP::m;
	G4double fTandarZ = 0.5*fTandarHeight + 0.5*fGroundSizeZ;

	// solids
	solidTandar = new G4Tubs("Tandar", fTandarRadius, fTandarRadius + fTandarThickness, 0.5 * fTandarHeight, 0, 360*deg);
	solidTandarTop = new G4Tubs("Top", 0, fTandarRadius + fTandarThickness, 0.5 * fTandarThickness, 0, 360*deg);

	// logical volumes and placements
	logTandar  = new G4LogicalVolume(solidTandar, Materials().Concrete, "logTandar", 0, 0, 0);
	physTandar = new G4PVPlacement(nullptr, G4ThreeVector(fTandarX, fTandarY, fTandarZ), logTandar, "physTandar", logicWorld, false, 0, fCheckOverlaps);
	
	logTandarTop  = new G4LogicalVolume(solidTandarTop, Materials().Concrete, "logTandarTop", 0, 0, 0);
	physTandarTop = new G4PVPlacement(nullptr, G4ThreeVector(fTandarX, fTandarY, 0.5*fGroundSizeZ + fTandarHeight + 0.5*fTandarThickness), logTandarTop, "physTandarTop", logicWorld, false, 0, fCheckOverlaps);


}


void
G4CasposoDetectorConstruction::PlaceDetector(Event& theEvent)
{
	
	const Event::Config &cfg = theEvent.GetConfig();
	// loop in detector vector
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {
		auto& currentDet = detIt->second;
		BuildDetector(logicWorld, currentDet, theEvent, cfg.fCheckOverlaps);
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
