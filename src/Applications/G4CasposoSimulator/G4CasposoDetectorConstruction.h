// definition of the G4CasposoDetectorConstruction class
#ifndef G4CasposoDetectorConstruction_h
#define G4CasposoDetectorConstruction_h 1

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

class G4CasposoDetectorConstruction : public G4VUserDetectorConstruction {
  
  public:
    G4CasposoDetectorConstruction(Event& theEvent);
    virtual ~G4CasposoDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

  private:

    void CreateElements();
    void CreateMaterials();
    
    void CreateWorld();
    G4VPhysicalVolume* CreateDetector();
    bool fCheckOverlaps = true;

    G4Element* elN = nullptr;
    G4Element* elO = nullptr;
    G4Element* elH = nullptr;
    G4Element* elSi = nullptr;
    G4Material* Air = nullptr;
    G4Material* SiO2 = nullptr;
    
    // solids
    G4Box* solidWorld = nullptr;
    
    // logical and physical volumes
    G4LogicalVolume* logicWorld = nullptr;
    G4PVPlacement*   physWorld  = nullptr;
    
    // size definitions
    G4double fWorldSizeX = 1*m;
    G4double fWorldSizeY = 1*m;
    G4double fWorldSizeZ = 1*m;

    Event& fEvent;
};


#endif