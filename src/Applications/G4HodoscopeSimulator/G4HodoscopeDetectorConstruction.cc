// implementation of the G4HodoscopeDetectorConstruction class
#include <iostream>

#include "G4HodoscopeDetectorConstruction.h"
#include "Materials.h"

#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

using namespace std;


G4HodoscopeDetectorConstruction::G4HodoscopeDetectorConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{ 
}

G4HodoscopeDetectorConstruction::~G4HodoscopeDetectorConstruction() 
	{ }

G4VPhysicalVolume*
G4HodoscopeDetectorConstruction::CreateDetector()
{

	CreateWorld();
	//CreateGround();
	PlaceDetector(fEvent);
	return physWorld;
}

void
G4HodoscopeDetectorConstruction::CreateWorld()
{

	// size definitions
	const Event::Config &cfg = fEvent.GetConfig();	

	// double fGroundSizeX = 5 * CLHEP::m;
	// double fGroundSizeY = 5 * CLHEP::m;
	// double fGroundSizeZ = 1 * CLHEP::m;

	double fWorldSizeX = 5 * CLHEP::m;
	double fWorldSizeY = 5 * CLHEP::m;
	double fWorldSizeZ = 5 * CLHEP::m;

	solidWorld  = new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld  =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);

	// lead bricks
	double fBrickSizeX = 10.5 * CLHEP::cm;
	double fBrickSizeY = 10.5 * CLHEP::cm;
	double fBrickSizeZ = 20.5 * CLHEP::cm;

	// solidBrick = new G4Box("LeadBrick", fBrickSizeX/2, fBrickSizeY/2, fBrickSizeZ/2);
	// G4VisAttributes cgray(G4Colour::Gray());
	// logicBrick = new G4LogicalVolume(solidBrick, Materials().Lead, "LeadBrick");
	// logicBrick->SetVisAttributes(cgray);
	// physBrick = new G4PVPlacement(nullptr, G4ThreeVector(0., 0., fBrickSizeZ/2 + 73*CLHEP::cm), logicBrick, "LeadBrick", logicWorld, false, 0, cfg.fCheckOverlaps);



}


void
G4HodoscopeDetectorConstruction::PlaceDetector(Event& theEvent)
{
	
	const Event::Config &cfg = theEvent.GetConfig();
	// loop in detector vector
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {
		auto& currentDet = detIt->second;
		BuildDetector(logicWorld, currentDet, theEvent, cfg.fCheckOverlaps);
	}

}

G4VPhysicalVolume* 
G4HodoscopeDetectorConstruction::Construct() 
{

	if (!physWorld) {
		//CreateElements();
		return CreateDetector();
	}
	return physWorld;

}
