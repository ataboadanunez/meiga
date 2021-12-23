#ifndef Mudulus_h
#define Mudulus_h 1

#include "G4SDManager.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"


#include "Event.h"
#include "Detector.h"
#include "Module.h"
#include "Materials.h"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;

class Mudulus {
	// description

	public:
		Mudulus();
		virtual ~Mudulus();

		void BuildDetector(G4LogicalVolume* logMother, Module& module, Event& theEvent, G4int fBarsPanel = 12, G4bool fCheckOverlaps = true);

		// getters
		G4LogicalVolume* GetLogicCasing() { return logicCasing; }

	private:
		// solids
		G4Box* solidCasing = nullptr;
		G4Box* solidCoating = nullptr;
		G4Box* solidScinBar = nullptr;

		G4Tubs* solidClad2 = nullptr;
		G4Tubs* solidClad1 = nullptr;
		G4Tubs* solidFiber = nullptr;

		G4Box* solidSiPM = nullptr;

		// logical volumes
		G4LogicalVolume* logicCasing = nullptr;
		G4LogicalVolume* logicCoating = nullptr;
		G4LogicalVolume* logicScinBar = nullptr;

		G4LogicalVolume* logicClad2 = nullptr;
		G4LogicalVolume* logicClad1 = nullptr;
		G4LogicalVolume* logicFiber = nullptr;

		G4LogicalVolume* logicSiPMl = nullptr;
		G4LogicalVolume* logicSiPMr = nullptr;

		// physical volumes
		G4PVPlacement* physCasing = nullptr;
		G4PVPlacement* physCoating = nullptr;
		G4PVPlacement* physScinBar = nullptr;

		G4PVPlacement* physClad2 = nullptr;
		G4PVPlacement* physClad1 = nullptr;
		G4PVPlacement* physFiber = nullptr;

		G4PVPlacement* physSiPMl = nullptr;
		G4PVPlacement* physSiPMr = nullptr;

		// following variables are read from Detector class
		G4double fCasingThickness;
		G4double fCasingSizeX;
		G4double fCasingSizeY;
		G4double fCasingSizeZ;

		G4double fCoatingThickness;

		G4double fBarWidth;
		G4double fBarLength;
		G4double fBarThickness; 

		G4double fCladdingThickness;
		G4double fFiberRadius;

		G4double fSiPMSizeX;
		G4double fSiPMSizeY;
		G4double fSiPMSizeZ;




};

#endif
