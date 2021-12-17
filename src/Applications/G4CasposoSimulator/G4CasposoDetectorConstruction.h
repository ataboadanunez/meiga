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

    G4double GetRockSizeX() const { return fRockSizeX; }
    G4double GetRockSizeY() const { return fRockSizeY; }
    G4double GetRockSizeZ() const { return fRockSizeZ; }


  private:
    //void SetDetectorParameters(); 
    void CreateElements();
    void CreateMaterials();
    
    void CreateWorld();
    void CreateRock();
    void CreateHall();
    G4VPhysicalVolume* CreateDetector();
    
    bool fCheckOverlaps = true;

    G4Element* elN = nullptr;
    G4Element* elO = nullptr;
    G4Element* elH = nullptr;
    G4Element* elSi = nullptr;
    G4Material* Air = nullptr;
    G4Material* Quartz = nullptr;
    G4Material* Water = nullptr;  

    // solids
    G4Box* solidWorld = nullptr;
    G4Box* solidRock = nullptr;
    G4Box* solidHall = nullptr;
    
    
    // logical and physical volumes
    G4LogicalVolume* logicWorld = nullptr;
    G4PVPlacement*   physWorld  = nullptr;
    
    G4LogicalVolume* logicRock = nullptr;
    G4PVPlacement*   physRock  = nullptr;

    G4LogicalVolume* logicHall = nullptr;
    G4PVPlacement*   physHall  = nullptr;

    // size definitions
    G4double fRockSizeX = 150*m;
    G4double fRockSizeY = 150*m;
    G4double fRockSizeZ = 150*m;

    G4double fWorldSizeX = fRockSizeX + 0.5*m;
    G4double fWorldSizeY = fRockSizeX + 0.5*m;
    G4double fWorldSizeZ = fRockSizeX + 0.5*m;

    G4double fHallSizeX = 25*m;
    G4double fHallSizeY = 3*m;
    G4double fHallSizeZ = 3*m;

    G4double fRockPosX = 0;
    G4double fRockPosY = 0;
    G4double fRockPosZ = 0;

    G4double fHallPosX = 0;
    G4double fHallPosY = 0;
    G4double fHallPosZ = fHallSizeZ - 150*m;

    Event& fEvent;
};


#endif
