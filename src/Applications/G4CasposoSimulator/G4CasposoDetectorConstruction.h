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

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;

// Detector construction class to define materials and geometry

class G4CasposoDetectorConstruction : public G4VUserDetectorConstruction {
	
	public:
		G4CasposoDetectorConstruction(Event& theEvent);
		virtual ~G4CasposoDetectorConstruction();
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
		G4double fGroundSizeX = 15 * CLHEP::m;
		G4double fGroundSizeY = 15 * CLHEP::m;
		G4double fGroundSizeZ = 15 * CLHEP::m;

		G4double fWorldSizeX = fGroundSizeX + 5 * CLHEP::m;
		G4double fWorldSizeY = fGroundSizeY + 5 * CLHEP::m;
		G4double fWorldSizeZ = fGroundSizeZ + 5 * CLHEP::m;

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
