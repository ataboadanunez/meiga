#ifndef Mudulus_h
#define Mudulus_h 1

#include "G4SDManager.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"

#include "Event.h"
#include "Detector.h"
#include "Materials.h"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;

class Mudulus {
	// description

public:
	Mudulus();
	virtual ~Mudulus();

	void BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOverlaps = false);

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

	G4Box* solidPixel = nullptr;

	// logical volumes
	G4LogicalVolume* logicCasing = nullptr;
	G4LogicalVolume* logicCoating = nullptr;
	G4LogicalVolume* logicScinBar = nullptr;
	
	G4LogicalVolume* logicClad2 = nullptr;
	G4LogicalVolume* logicClad1 = nullptr;
	G4LogicalVolume* logicFiber = nullptr;

	G4LogicalVolume* logicPixelL = nullptr;
	G4LogicalVolume* logicPixelR = nullptr;

	// physical volumes
	G4PVPlacement* physCasing = nullptr;
	G4PVPlacement* physCoating = nullptr;
	G4PVPlacement* physScinBar = nullptr;

	G4PVPlacement* physClad2 = nullptr;
	G4PVPlacement* physClad1 = nullptr;
	G4PVPlacement* physFiber = nullptr;

	G4PVPlacement* physPixelL = nullptr;
	G4PVPlacement* physPixelR = nullptr;

	// detector properties are fixed for this detector.
	G4int nPanels = 3;
	G4double fDistanceBtwPanels = 1 * CLHEP::m;

	G4int nBars = 12;
	G4double fBarWidth = 41 * CLHEP::mm;
	G4double fBarLength = 495 * CLHEP::mm;
	G4double fBarThickness = 10 * CLHEP::mm;
	G4double fCoatingThickness = 0.25 * CLHEP::mm;
	G4double fHalfWidth = 0.5 * nBars * (fBarWidth + fCoatingThickness); 

	G4double fCladdingThickness = 0.10 * CLHEP::mm;
	G4double fFiberRadius = 1.2 * CLHEP::mm;
	G4double fFiberLength = 1970 * CLHEP::mm;
	//G4double fFiberLength = fBarLength;
	G4double fCasingThickness = 2 * CLHEP::mm;
	G4double fCasingSizeX = fFiberLength + 570 * CLHEP::mm;
	G4double fCasingSizeY = fFiberLength + 570 * CLHEP::mm;
	G4double fCasingSizeZ = fBarThickness * 4 + fCasingThickness; // (x2 number of panels)

	G4double fPMTSizeX = 1.0 * CLHEP::mm;
	G4double fPMTSizeY = 1.0 * CLHEP::mm;
	G4double fPMTSizeZ = 0.1 * CLHEP::mm;


	G4double GetFiberXCoordinate(G4int barId);
};

#endif