// definition of the G4MudulusDetectorConstruction class
#ifndef G4MudulusDetectorConstruction_h
#define G4MudulusDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4SDManager.hh"
#include "globals.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "Event.h"
#include "Detector.h"
#include "Module.h"



class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;

// Detector construction class to define materials and geometry

class G4MudulusDetectorConstruction : public G4VUserDetectorConstruction {
  
  public:
    G4MudulusDetectorConstruction(Event& theEvent);
    virtual ~G4MudulusDetectorConstruction();
    virtual G4VPhysicalVolume* Construct();

    G4double GetGroundSizeX() const { return fGroundSizeX; }
    G4double GetGroundSizeY() const { return fGroundSizeY; }
    G4double GetGroundSizeZ() const { return fGroundSizeZ; }

  private:
    
    void CreateWorld();
    void CreateGround();
    void PlaceDetector(Event& theEvent);  
    G4VPhysicalVolume* CreateDetector();

    bool fCheckOverlaps = true;
    
    // solids
    G4Box* solidWorld = nullptr;
    G4Box* solidGround = nullptr;
    
    // logical and physical volumes
    G4LogicalVolume* logicWorld = nullptr;
    G4PVPlacement*   physWorld  = nullptr;
    
    G4LogicalVolume* logicGround = nullptr;
    G4PVPlacement*   physGround  = nullptr;

    // size definitions
    
    G4double fGroundSizeX = 5*m;
    G4double fGroundSizeY = 5*m;
    G4double fGroundSizeZ = 1*m;

    G4double fWorldSizeX = fGroundSizeX + 5*m;
    G4double fWorldSizeY = fGroundSizeY + 5*m;
    G4double fWorldSizeZ = fGroundSizeZ + 5*m;

    Event& fEvent;
};


#endif