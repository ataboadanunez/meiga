// definition of the G4RockDetectorConstruction class
#ifndef G4RockDetectorConstruction_h
#define G4RockDetectorConstruction_h 1

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

class G4RockDetectorConstruction : public G4VUserDetectorConstruction {
  
  public:
    G4RockDetectorConstruction(Event& theEvent);
    virtual ~G4RockDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    
    G4double GetRockDepth() const { return fRockDepth; }
  	G4double GetRockRadius() const { return fRockRadius; }

  private:
  	//void SetDetectorParameters();
    void CreateElements();
    void CreateMaterials();
    
    void CreateWorld();
    void CreateHall();
    G4VPhysicalVolume* CreateDetector();
    void CreateModule(Module& mod);
    void ReadModuleList(std::string fModuleList);

    // different methods for each material
    void CreateAir();
    void CreateRock();
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
    G4Material* RockMaterial = nullptr;
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
    G4Tubs* solidRock = nullptr;
    G4Box* solidHall = nullptr;
    G4Box* solidCasing = nullptr;
    G4Box* solidCoat = nullptr;
    G4Box* solidBar  = nullptr;
    G4Tubs* solidFiber = nullptr;
    G4Tubs* solidClad1 = nullptr;
    G4Tubs* solidClad2 = nullptr;
    G4Box*  solidSiPM  = nullptr;

    // logical and physical volumes
    G4LogicalVolume* logicWorld = nullptr;
    G4PVPlacement*   physWorld  = nullptr;
    
    G4LogicalVolume* logicRock  = nullptr;
    G4VPhysicalVolume* physRock = nullptr;
    
    G4LogicalVolume* logicHall  = nullptr;
    G4VPhysicalVolume* physHall = nullptr;
    
    G4LogicalVolume* logicCasing = nullptr;
    G4VPhysicalVolume* physCasing = nullptr;
    
    G4LogicalVolume* logicCoat  = nullptr;
    G4VPhysicalVolume* physCoat = nullptr;
    
    G4LogicalVolume* logicBar   = nullptr;
    G4VPhysicalVolume* physBar  = nullptr;

    G4LogicalVolume* logicFiber = nullptr;
    G4VPhysicalVolume* physFiber = nullptr;
    
    G4LogicalVolume* logicClad1 = nullptr;
    G4VPhysicalVolume* physClad1 = nullptr;
    
    G4LogicalVolume* logicClad2 = nullptr;
    G4VPhysicalVolume* physClad2 = nullptr;

    G4LogicalVolume* logicSiPM  = nullptr;
    G4VPhysicalVolume* physSiPM  = nullptr;

    // 

    // por ahora hard-coded aqui, eventualmente una funcion seteara los parametros a partir de un config file
    G4double fRockRadius = 150*m;
    G4double fRockDepth  = 1500*m;
    
    G4double fWorldSizeX = fRockRadius + 0.5*m;
    G4double fWorldSizeY = fRockRadius + 0.5*m;
    G4double fWorldSizeZ = fRockDepth  + 0.5*m;
    G4double fHallSizeX = 5*m;
    G4double fHallSizeY = 5*m;
    G4double fHallSizeZ = 5*m;

    // read from Detector class
    G4double fBarWidth;
    G4double fBarLength;
    G4double fBarThickness;
    G4double fCoatingThickness;
    G4double fCasingThickness;
    G4double fFiberRadius;
    G4double fCladdingThickness;
    G4double fCasingSizeX;
    G4double fCasingSizeY;
    G4double fCasingSizeZ;
    G4double fSiPMSizeX;
    G4double fSiPMSizeY;
    G4double fSiPMSizeZ;

    //G4int nTopBars = 2;
  	//G4int nBotBars = 2;

    // por ahora centrar todo en origen de coordenadas de Geant4
    G4double fRockPosX = 0;
    G4double fRockPosY = 0;
    G4double fRockPosZ = 0;
    G4double fHallPosX = 0;
    G4double fHallPosY = 0;
    //G4double fHallPosY = -fRockHeight + fHallSizeY;
    //G4double fHallPosY = -1*m;
    G4double fHallPosZ = -fRockDepth + fHallSizeZ;
    
    // according to Auger UMD simulation this is 20/keV
    G4double fScintillationYield = 9/keV; // DOI:10.15407/fm20.03.304

    
    std::string fModuleId = "0";

    Event& fEvent;
};


#endif