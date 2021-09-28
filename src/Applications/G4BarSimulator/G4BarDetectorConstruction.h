#ifndef G4BarDetectorConstruction_h
#define G4BarDetectorConstruction_h 1

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

class G4BarDetectorConstruction : public G4VUserDetectorConstruction {
  
  public:
    G4BarDetectorConstruction(Event& theEvent);
    virtual ~G4BarDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

  private:
  	//void SetDetectorParameters();
    void CreateElements();
    void CreateMaterials();
    
    void CreateWorld();
    G4VPhysicalVolume* CreateDetector();

    // different methods for each material
    void CreateAir();
    void CreatePolystyrene();
    void CreateScintillator();
    void SetScinPropertyTable();
    void CreateWLS();
    void CreatePyrex();

    //G4ThreeVector ToG4Vector(const std::vector<double>* v, const double unit);

    bool fCheckOverlaps = true;

    G4Element* elN = nullptr;
    G4Element* elO = nullptr;
    G4Element* elH = nullptr;
    G4Element* elC = nullptr;
    G4Element* elSi = nullptr;
    G4Element* elTi = nullptr;
    G4Element* elB  = nullptr;
    G4Element* elNa = nullptr;
    G4Material* Air = nullptr;
    G4Material* SiO2 = nullptr;
    G4Material* TiO2 = nullptr;
    G4Material* B2O2 = nullptr;
    G4Material* Na2O = nullptr;
    G4Material* BarMaterial = nullptr;
    G4Material* Polystyrene = nullptr;
    G4Material* PPO = nullptr;
    G4Material* POPOP = nullptr;
    G4Material* ScinCoat = nullptr;
    G4Material* ScinMat = nullptr;
    G4Material* PMMA = nullptr;
    G4Material* Pethylene = nullptr;
    G4Material* FPethylene = nullptr;
    G4Material* Pyrex = nullptr;
    G4OpticalSurface* ScinOptSurf = nullptr;

    // solids
    G4Box* solidWorld = nullptr;
    G4Box* solidCasing = nullptr;
    G4Box* solidCoat = nullptr;
    G4Box* solidBar  = nullptr;
    G4Tubs* solidFiber = nullptr;
    G4Tubs* solidFiberExt = nullptr;
    G4Tubs* solidClad1 = nullptr;
    G4Tubs* solidClad1Ext = nullptr;
    G4Tubs* solidClad2 = nullptr;
    G4Tubs* solidHole  = nullptr;
    G4Tubs* solidClad2Ext = nullptr;
    G4Box*  solidPMT  = nullptr;

    // logical and physical volumes
    G4LogicalVolume* logicWorld = nullptr;
    G4PVPlacement*   physWorld  = nullptr;
    
    G4LogicalVolume* logicCasing = nullptr;
    G4VPhysicalVolume* physCasing = nullptr;
    
    G4LogicalVolume* logicCoat  = nullptr;
    G4VPhysicalVolume* physCoat = nullptr;
    
    G4LogicalVolume* logicBar   = nullptr;
    G4VPhysicalVolume* physBar  = nullptr;

    G4LogicalVolume* logicFiber = nullptr;
    G4VPhysicalVolume* physFiber = nullptr;

    G4LogicalVolume* logicHole = nullptr;
    G4VPhysicalVolume* physHole = nullptr;

    G4LogicalVolume* logicFiberExt1 = nullptr;
    G4VPhysicalVolume* physFiberExt1 = nullptr;

    G4LogicalVolume* logicFiberExt2 = nullptr;
    G4VPhysicalVolume* physFiberExt2 = nullptr;
    
    G4LogicalVolume* logicClad1 = nullptr;
    G4VPhysicalVolume* physClad1 = nullptr;

    G4LogicalVolume* logicClad1Ext1 = nullptr;
    G4VPhysicalVolume* physClad1Ext1 = nullptr;

    G4LogicalVolume* logicClad1Ext2 = nullptr;
    G4VPhysicalVolume* physClad1Ext2 = nullptr;
    
    G4LogicalVolume* logicClad2 = nullptr;
    G4VPhysicalVolume* physClad2 = nullptr;

    G4LogicalVolume* logicClad2Ext1 = nullptr;
    G4VPhysicalVolume* physClad2Ext1 = nullptr;

    G4LogicalVolume* logicClad2Ext2 = nullptr;
    G4VPhysicalVolume* physClad2Ext2 = nullptr;

    G4LogicalVolume* logicPMT  = nullptr;
    G4VPhysicalVolume* physPMT1  = nullptr;
    G4VPhysicalVolume* physPMT2  = nullptr;



    // 

    G4double fWorldSizeX = 2.5*m;
    G4double fWorldSizeY = 2.5*m;
    G4double fWorldSizeZ = 2.5*m;

    G4double fBarWidth = 4.1*cm;
    G4double fBarLength = 2*m;
    G4double fBarThickness = 1*cm;
    G4double fCoatingThickness = 0.25*mm;
    G4double fCasingThickness = 2*mm;
    G4double fFiberRadius = 1.2*mm;
    G4double fCladdingThickness = 0.10*mm;
    G4double fHoleThickness = 0.1*mm;
    G4double fCasingSizeX;
    G4double fCasingSizeY;
    G4double fCasingSizeZ;
    G4double fPMTSizeX = 1.3*mm;
    G4double fPMTSizeY = 1.3*mm;
    G4double fPMTSizeZ = 0.1*mm;

    G4double fBarPosX = 0;
    G4double fBarPosY = 0;
    G4double fBarPosZ = 0;

    // according to Auger UMD simulation this is 20/keV
    G4double fScintillationYield = 9/keV; // DOI:10.15407/fm20.03.304

    Event& fEvent;
};


#endif