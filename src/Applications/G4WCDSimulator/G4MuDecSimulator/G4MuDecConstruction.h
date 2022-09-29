// definition of the G4MuDecConstruction class
#ifndef G4MuDecConstruction_h
#define G4MuDecConstruction_h 1

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

class G4MuDecConstruction : public G4VUserDetectorConstruction {
	
	public:
		G4MuDecConstruction(Event& theEvent);
		virtual ~G4MuDecConstruction();
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
#warning "temporary fix. Ground and World size should be set by DetectorSimData"
		// size definitions
		G4double fGroundSizeX = 10 * CLHEP::m;
		G4double fGroundSizeY = 10 * CLHEP::m;
		G4double fGroundSizeZ = 1 * CLHEP::m;

		G4double fWorldSizeX = fGroundSizeX + 0.5 * CLHEP::m;
		G4double fWorldSizeY = fGroundSizeY + 0.5 * CLHEP::m;
		G4double fWorldSizeZ = fGroundSizeZ + 5 * CLHEP::m;

		Event& fEvent;
};


#endif
