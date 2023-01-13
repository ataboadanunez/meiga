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
	G4double fWorldSizeX = 500 * CLHEP::m;
	G4double fWorldSizeY = 500 * CLHEP::m;
	G4double fWorldSizeZ = 500 * CLHEP::m;

	solidWorld  = new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld  =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);

}

void
G4CasposoDetectorConstruction::CreateGround()
{
	
	const Event::Config &cfg = fEvent.GetConfig();	
	G4double fGroundSizeX = 495 * CLHEP::m;
	G4double fGroundSizeY = 495 * CLHEP::m;
	G4double fGroundSizeZ = 1 * CLHEP::m;

	// according to google maps
	G4double fDistance2Detector = 195 * CLHEP::m;

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
	physGround  =  new G4PVPlacement(nullptr, G4ThreeVector(), logicGround, "Ground", logicWorld, false, 0, cfg.fCheckOverlaps);

                        
	/*    TANDAR             Stairs
	                   		 __________
	  		___________     | ________ |
	 		 /___________\    ||        ||
      //           \\   ||        ||
     //             \\  ||        ||
    //               \\ ||        ||
    ||               || ||        ||
    ||       .       || ||        ||
    ||               || ||        ||
    \\               // ||        ||
     \\             //  ||        ||
      \\___________//   ||________||
       \___________/    | ________ |

             X
             ^
             |
             |
             |
           Z(o)-----> Y

	
	Tandar Radius = 5.9 m
	Tandar wall thickness = 0.5 m
	Tandar Height = 73 m

	Stairs building X = 10.5 m
	Stairs building Y = 7.8 m
	Stairs building thickness = 0.3 * 2 m
	Stairs building height = 73 m
	

	*/


	// TANDAR dimensions
	G4double fDetectorLocationHeight = 6 *CLHEP::m;
	G4double fTandarHeight = 73 * CLHEP::m - fDetectorLocationHeight;
	G4double fTandarRadius = 5.9 * CLHEP::m;
	G4double fTandarThickness = 0.5 * CLHEP::m;
	
	// dimensions of stairs building
	G4double fStairsWidthX = (2.9 + 0.3 + 2.8 + 0.3 + 4.2) * CLHEP::m;
	G4double fStairsWidthY = (2.5 + 0.3 + 2.3 + 0.3 + 2.4) * CLHEP::m;
	G4double fStairsThickness = 0.3 * CLHEP::m;
	G4double fStairsHeight = fTandarHeight;
	// position of TANDAR
	// TANDAR origin located w.r.t detector position
	G4double fTandarX = 0 * CLHEP::m;
	G4double fTandarY = fDistance2Detector;
	G4double fTandarZ = 0.5*fTandarHeight + 0.5*fGroundSizeZ;

	// position of Stairs building
	G4double fStairsX = 0 * CLHEP::m;
	G4double fStairsY = fTandarY - (fTandarRadius + fTandarThickness + fStairsThickness + fStairsWidthY * 0.5);
	G4double fStairsZ = fTandarZ;

	// solids
	solidTandar = new G4Tubs("Tandar", fTandarRadius, fTandarRadius + fTandarThickness, 0.5 * fTandarHeight, 0, 360*deg);
	solidTandarTop = new G4Tubs("Top", 0, fTandarRadius + fTandarThickness, 0.5 * fTandarThickness, 0, 360*deg);
	solidStairsIn = new G4Box("StairsIn", fStairsWidthX * 0.5, fStairsWidthY * 0.5, fStairsHeight * 0.5);
	solidStairsOut = new G4Box("StairsOut", fStairsWidthX * 0.5 + fStairsThickness * 0.5, fStairsWidthY * 0.5 + fStairsThickness * 0.5, fStairsHeight * 0.5);

	// logical volumes and placements
	logTandar  = new G4LogicalVolume(solidTandar, Materials().Concrete, "logTandar", 0, 0, 0);
	physTandar = new G4PVPlacement(nullptr, G4ThreeVector(fTandarX, fTandarY, fTandarZ), logTandar, "physTandar", logicWorld, false, 0, cfg.fCheckOverlaps);

	logTandarTop  = new G4LogicalVolume(solidTandarTop, Materials().Concrete, "logTandarTop", 0, 0, 0);
	physTandarTop = new G4PVPlacement(nullptr, G4ThreeVector(fTandarX, fTandarY, 0.5*fGroundSizeZ + fTandarHeight + 0.5*fTandarThickness), logTandarTop, "physTandarTop", logicWorld, false, 0, cfg.fCheckOverlaps);

	logStairsIn = new G4LogicalVolume(solidStairsIn, Materials().Air, "logStairsIn", 0, 0, 0);
	G4VisAttributes blue(G4Colour::Blue());
	logStairsIn->SetVisAttributes(blue);
	physStairsIn = new G4PVPlacement(nullptr, G4ThreeVector(fStairsX, fStairsY, fStairsZ), logStairsIn, "physStairsIn", logicWorld, false, 0, cfg.fCheckOverlaps);

	logStairsOut = new G4LogicalVolume(solidStairsOut, Materials().Concrete, "logStairsOut", 0, 0, 0);
	physStairsOut = new G4PVPlacement(nullptr, G4ThreeVector(fStairsX, fStairsY, fStairsZ), logStairsOut, "physStairsOut", logicWorld, false, 0, cfg.fCheckOverlaps);


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
