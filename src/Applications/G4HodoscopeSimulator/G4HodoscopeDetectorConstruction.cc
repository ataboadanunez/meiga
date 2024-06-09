// implementation of the G4HodoscopeDetectorConstruction class
#include <iostream>

#include "G4HodoscopeDetectorConstruction.h"
#include "Materials.h"

#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

using namespace std;


G4HodoscopeDetectorConstruction::G4HodoscopeDetectorConstruction(Event& aEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(aEvent)
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
	const Event::Config &cfg = fEvent.GetConfig();	
	// world size definitions
	fWorldSizeX = 5 * CLHEP::m;
	fWorldSizeY = 5 * CLHEP::m;
	fWorldSizeZ = 5 * CLHEP::m;

	solidWorld  = new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Materials().Air, "World");
	physWorld  =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);
}


void
G4HodoscopeDetectorConstruction::PlaceDetector(Event& aEvent)
{
	
	const Event::Config &cfg = aEvent.GetConfig();
	// loop in detector vector
	for (auto &pair : aEvent.DetectorRange()) {
		auto& currentDet = *(pair.second);
		currentDet.BuildDetector(logicWorld, aEvent, cfg.fCheckOverlaps);
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
