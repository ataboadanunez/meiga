// definition of the G4RockDetectorConstruction class

#ifndef G4RockDetectorConstruction_h
#define G4RockDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
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



class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;

// Detector construction class to define materials and geometry

class G4RockDetectorConstruction : public G4VUserDetectorConstruction {
  
  public:
    G4RockDetectorConstruction();
    virtual ~G4RockDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    const G4VPhysicalVolume* GetSiPM() const;
    const G4VPhysicalVolume* GetFiber() const;
    const G4VPhysicalVolume* GetScinBar() const;

  private:
    void CreateElements();
    void CreateMaterials();
    void CreateSolids();
    void CreateWorld();
    void CreateHall();
    G4VPhysicalVolume* CreateDetector();
    void AssembleDetector();
    

    // different methods for each material
    void CreateAir();
    void CreateRock();
    void CreatePolystyrene();
    void CreateScintillator();
    void SetScinPropertyTable();
    void CreateWLS();

    bool fCheckOverlaps = true;

    G4Element* elN = nullptr;
    G4Element* elO = nullptr;
    G4Element* elH = nullptr;
    G4Element* elC = nullptr;
    G4Element* elSi = nullptr;
    G4Element* elTi = nullptr;
    G4Material* Air = nullptr;
    G4Material* SiO2 = nullptr;
    G4Material* TiO2 = nullptr;
    G4Material* RockMaterial = nullptr;
    G4Material* Polystyrene = nullptr;
    G4Material* PPO = nullptr;
    G4Material* POPOP = nullptr;
    G4Material* ScinCoat = nullptr;
    G4Material* ScinMat = nullptr;
    G4Material* PMMA = nullptr;
    G4Material* Pethylene = nullptr;
    G4Material* FPethylene = nullptr;
    G4OpticalSurface* ScinOptSurf = nullptr;

    // solids
    G4Box* solidWorld = nullptr;
    G4Box* solidRock = nullptr;
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
    G4LogicalVolume* logicFiber = nullptr;
    G4LogicalVolume* logicClad1 = nullptr;
    G4LogicalVolume* logicClad2 = nullptr;
    G4LogicalVolume* logicSiPM  = nullptr;
    G4VPhysicalVolume* physBar  = nullptr;
    G4VPhysicalVolume* physFiber = nullptr;
    G4VPhysicalVolume* physClad1 = nullptr;
    G4VPhysicalVolume* physClad2 = nullptr;
    G4VPhysicalVolume* physSiPM  = nullptr;
    // 

    // por ahora hard-coded aqui, eventualmente una funcion seteara los parametros a partir de un config file
    G4double fRockSizeX = 3*m;
    G4double fRockSizeY = 3*m;
    G4double fRockSizeZ = 3*m;
    G4double fWorldSizeX = 5*m;
    G4double fWorldSizeY = 5*m;
    G4double fWorldSizeZ = 5*m;
    G4double fHallSizeX = 1*m;
    G4double fHallSizeY = 1*m;
    G4double fHallSizeZ = 1*m;
    G4double fBarWidth = 41*mm;
    G4double fBarLength = 2*fBarWidth;
    G4double fBarThickness = 10*mm;
    G4double fFiberRadius = 1.2*mm;
    G4double fCasingThickness = 2*mm;
    G4double fCladdingThickness = 0.10*mm;
    G4double fCoatingThickness  = 0.25*mm;
    G4double fSiPMSizeX = 1.*mm;
    G4double fSiPMSizeY = 1.*mm; 
    G4double fSiPMSizeZ = 0.1*mm;

    // por ahora centrar todo en origen de coordenadas de Geant4
    G4double fRockPosX = 0;
    G4double fRockPosY = 0;
    G4double fRockPosZ = 0;
    G4double fHallPosX = 0;
    G4double fHallPosY = 0;
    G4double fHallPosZ = 0;
    
    G4double fScintillationYield = 50/MeV; // according to Auger UMD simulation
    
};

// inline functions (following Geant4 example B4)

inline const G4VPhysicalVolume* G4RockDetectorConstruction::GetSiPM() const { 
  return physSiPM; 
}

inline const G4VPhysicalVolume* G4RockDetectorConstruction::GetFiber() const { 
  return physFiber; 
}

inline const G4VPhysicalVolume* G4RockDetectorConstruction::GetScinBar() const { 
  return physBar; 
}


#endif