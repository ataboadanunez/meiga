#include "G4MDetectorConstruction.h"
#include "Materials.h"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4UnitsTable.hh"

G4MDetectorConstruction::G4MDetectorConstruction(Event& /*aEvent*/)
    : G4VUserDetectorConstruction()
{
}

G4VPhysicalVolume* 
G4MDetectorConstruction::Construct() 
{
	if(!fPhysWorld) {
        G4Box *solid = new G4Box("World", 0.5 * CLHEP::m, 0.5 * CLHEP::m, 0.5 * CLHEP::m);
        G4LogicalVolume *logic = new G4LogicalVolume(solid, Materials().Air, "World");
        fPhysWorld = new G4PVPlacement(nullptr, G4ThreeVector(), "World", 
                                        logic, 0, false, 0, false);
    } 
    return fPhysWorld;
}