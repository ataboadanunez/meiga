// implementation of the G4ExDetectorConstruction class
#include "G4ExDetectorConstruction.h"
#include "Materials.h"

#include "Musaic.h"

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
  /***
   The function PlaceDetector() should be a replacement of ReadModuleList in G4RockSimulator.
   It should call an specific detector class (type) from G4Models and place it at a given
   position in a given mother volume.

  ***/
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
  
  Detector& detector = theEvent.GetDetector();
  int nModules = detector.GetNModules();
  cout << "Number of simulated modules = " << nModules << endl;
  // loop in ModulesRange
  for (auto modIt = detector.ModulesRange().begin(); modIt != detector.ModulesRange().end(); modIt++) {

    auto& currentMod = modIt->second;
    Musaic().BuildDetector(logicGround, currentMod, theEvent);
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