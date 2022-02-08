#include <iostream>

#include "Mudulus.h"
#include "Geometry.h"
#include "G4MOptDeviceAction.h"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

using namespace std;


void 
Mudulus::BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOverlaps)
{
	
	// the following variables need to be declared here as
	// BuildDetector() is a static method of the Class.

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

	// detector properties are fixed for this detector.
	G4int nPanels = 3;
	G4double fDistanceBtwPanels = 1 * CLHEP::m;

	const G4int nBars = 12;
	const G4double fBarLength   = 495 * CLHEP::mm;
	const G4double fFiberLength = 1970 * CLHEP::mm;
	const G4double fBarThickness = 10 * CLHEP::mm;
	const G4double fCoatingThickness = 0.25 * CLHEP::mm;
	const G4double fBarWidth = 41 * CLHEP::mm;
	const G4double fHalfWidth = 0.5 * nBars * (fBarWidth + fCoatingThickness); 

	const G4double fCladdingThickness = 0.10 * CLHEP::mm;
	const G4double fFiberRadius = 1.2 * CLHEP::mm;
	const G4double fCasingThickness = 2 * CLHEP::mm;
	const G4double fCasingSizeX = fFiberLength + 570 * CLHEP::mm;
	const G4double fCasingSizeY = fFiberLength + 570 * CLHEP::mm;
	const G4double fCasingSizeZ = fBarThickness * 4 + fCasingThickness; // (x2 number of panels)

	const G4double fPMTSizeX = 1.0 * CLHEP::mm;
	const G4double fPMTSizeY = 1.0 * CLHEP::mm;
	const G4double fPMTSizeZ = 0.1 * CLHEP::mm;

	G4ThreeVector  detectorPos = Geometry::ToG4Vector(detector.GetDetectorPosition(), 1.);

	int detectorId = detector.GetId();

	OptDevice pmt = detector.GetOptDevice(OptDevice::ePMT);
	
	ostringstream namedetector;
	namedetector.str("");
	namedetector << "Mudulus";
	cout << "G4Models::Mudulus: Building detector " << namedetector.str();
	cout << " (" << detectorId << ")";
	cout << " with " << pmt.GetName() << ". " << endl;

	/*************************************************
		
		Geant4 Volume construction
		
		This detector is composed by three panels of
		scintillator bars. Each panel consists of two
		layers of 12 bars each crossed constituting a grid.
		Each bar is traversed by one optical (WLS) fiber
		which collects and transports scintillation light
		to a 48 multipixel PMT. 

		In this detector construction, each PMT pixel is
		simulated as a Pyrex box of 1.3 x 1.3 x 0.1 mm^3
		which is intended just for light collection.

		PMT response is computed using the 
		G4MOptDeviceAction class.

	*************************************************/ 


	// solids
	// Casing
	solidCasing = new G4Box("Casing", 0.5*fCasingSizeX, 0.5*fCasingSizeY, 0.5*fCasingSizeZ);
	// Bars: Coating + Scintillator bar
	solidCoating  	= new G4Box("BarCoating", 0.5*fBarLength + fCoatingThickness, 0.5*fBarWidth + fCoatingThickness, 0.5*fBarThickness + fCoatingThickness);
	solidScinBar   	= new G4Box("BarScin", 0.5*fBarLength, 0.5*fBarWidth, 0.5*fBarThickness);
	// Fiber: Cladding (external & internal) + fiber core
	solidClad2  = new G4Tubs("Clad2", 0, fFiberRadius, 0.5*fFiberLength, 0, 360*deg);
	solidClad1  = new G4Tubs("Clad1", 0, fFiberRadius - fCladdingThickness, 0.5*fFiberLength, 0, 360*deg);
	solidFiber  = new G4Tubs("Fiber", 0, fFiberRadius - 2*fCladdingThickness, 0.5*fFiberLength, 0, 360*deg);
	// PMT
	solidPixel   = new G4Box("PMT", fPMTSizeX, fPMTSizeY, fPMTSizeZ);
	new G4LogicalSkinSurface("BarCoating", logicCoating, Materials().ScinOptSurf);

	// useful rotation matrix for bars orientation
	G4RotationMatrix* rotationFiber = new G4RotationMatrix();
	G4RotationMatrix* rotationBot = new G4RotationMatrix();
	rotationFiber->rotateY(M_PI/2);
	rotationBot->rotateZ(M_PI/2);

	// color definitions for visualization
	G4VisAttributes green(G4Colour::Green());
	G4VisAttributes blue(G4Colour::Blue());
	G4VisAttributes black(G4Colour::Black());

	// assemble Mudulus detector
	G4SDManager* const sdMan = G4SDManager::GetSDMpointer();

	// loop over casings (volume which contains the panels)
	for (G4int pIt=0; pIt<nPanels; ++pIt) {

		G4double fPanelPosZ;
		if (pIt==0)
			fPanelPosZ = 0;
		else if (pIt==1)
			fPanelPosZ = fDistanceBtwPanels;
		else if (pIt==2)
			fPanelPosZ = -1*fDistanceBtwPanels;

		G4double fPanelPosX = detectorPos.getX();
		G4double fPanelPosY = detectorPos.getY();
		fPanelPosZ += detectorPos.getZ();
		G4ThreeVector panelPosition = G4ThreeVector(fPanelPosX, fPanelPosY, fPanelPosZ);

		G4int panelId = pIt+1;
		G4String nameCasing = "Casing_"+to_string(panelId);
		logicCasing = new G4LogicalVolume(solidCasing, Materials().Air, nameCasing, 0, 0, 0);
		physCasing  = new G4PVPlacement(nullptr, panelPosition, logicCasing, nameCasing, logMother, false, detectorId, fCheckOverlaps);
		

		// bars of the top panel
		for (G4int bIt=0; bIt<nBars; ++bIt) {

			// bar position (w.r.t casing)
			G4String gridName = "X";
			int barId = bIt+1;
			G4double fBarPosX =  - 0.5*fBarWidth;//0 * CLHEP::mm;
			G4double fBarPosY = bIt*(fBarWidth + 2*fCoatingThickness);
			G4double fBarPosZ = (0.5*fBarThickness * 2 + fCasingThickness) / 2;
			G4ThreeVector barPosition = G4ThreeVector(fBarPosX, fBarPosY-fHalfWidth, fBarPosZ);

			// fiber position (w.r.t scintillator bar)
			G4double fFiberPosX = GetFiberXCoordinate(bIt, fBarLength, fFiberLength);
			G4double fFiberPosY = 0 * CLHEP::mm;
			G4double fFiberPosZ = -0.5*fBarThickness + fCoatingThickness + fFiberRadius + fCladdingThickness;
			G4ThreeVector fiberPosition = G4ThreeVector(fFiberPosX, fFiberPosY, fFiberPosZ);

			// PMT position (w.r.t fiber)
			G4double fPMTPosX = 0 * CLHEP::mm;
			G4double fPMTPosY = 0 * CLHEP::mm;
			G4double fPMTPosZ = 0.5*fFiberLength + 0.5*fPMTSizeZ;
			// PMT indexing
			G4int pixelLId = 100*panelId + barId; // (101...112, 201...212, 301...312)
			G4int pixelRId = 100*panelId + barId + nBars; // (113...124, 213...224, 313...324)
			

			G4String nameCoating = "BarCoating_"+gridName+"_"+to_string(barId);
			G4String nameScinBar = "BarScin_"+gridName+"_"+to_string(barId);
			G4String nameClad2 = "FiberClad2_"+gridName+"_"+to_string(barId);
			G4String nameClad1 = "FiberClad1_"+gridName+"_"+to_string(barId);
			G4String nameFiber = "Fiber_"+gridName+"_"+to_string(barId);
			G4String namePixelL = "PMT_"+gridName+"_"+to_string(pixelLId);
			G4String namePixelR = "PMT_"+gridName+"_"+to_string(pixelRId);
			// register PMTs in the detector class
			detector.MakeOptDevice(pixelLId, OptDevice::ePMT);
			detector.MakeOptDevice(pixelRId, OptDevice::ePMT);
			

			// logical volumes
			logicCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, nameCoating, 0, 0, 0);
			logicScinBar  = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, nameScinBar, 0, 0, 0);
			logicClad2 = new G4LogicalVolume(solidClad2, Materials().FPethylene, nameClad2, 0, 0, 0);
			logicClad2->SetVisAttributes(green);
			logicClad1 = new G4LogicalVolume(solidClad1, Materials().Pethylene, nameClad1, 0, 0, 0);
			logicClad1->SetVisAttributes(green); 
			logicFiber = new G4LogicalVolume(solidFiber, Materials().PMMA, nameFiber, 0, 0, 0);
			logicFiber->SetVisAttributes(green); 
			logicPixelL = new G4LogicalVolume(solidPixel, Materials().Pyrex, namePixelL, 0, 0, 0);
			logicPixelL->SetVisAttributes(blue); 
			logicPixelR = new G4LogicalVolume(solidPixel, Materials().Pyrex, namePixelR, 0, 0, 0);
			logicPixelR->SetVisAttributes(blue);

			// physical volumes
			physCoating = new G4PVPlacement(0, barPosition, nameCoating, logicCoating, physCasing, false, barId, fCheckOverlaps);
			physScinBar = new G4PVPlacement(0, G4ThreeVector(), nameScinBar, logicScinBar, physCoating, false, barId, fCheckOverlaps);
			physClad2 = new G4PVPlacement(rotationFiber, fiberPosition, nameClad2, logicClad2, physScinBar, false, barId, fCheckOverlaps);
			physClad1 = new G4PVPlacement(0, G4ThreeVector(), nameClad1, logicClad1, physClad2, false, barId, fCheckOverlaps);
			physFiber = new G4PVPlacement(0, G4ThreeVector(), nameFiber, logicFiber, physClad1, false, barId, fCheckOverlaps);
			new G4PVPlacement(0, G4ThreeVector(fPMTPosX, fPMTPosY, fPMTPosZ), namePixelL, logicPixelL, physFiber, false, pixelLId, fCheckOverlaps);
			new G4PVPlacement(0, G4ThreeVector(fPMTPosX, fPMTPosY, -1*fPMTPosZ), namePixelR, logicPixelR, physFiber, false, pixelRId, fCheckOverlaps);

			// registration of PMTs as Sensitive Detectors
			G4MOptDeviceAction* const PixelTopL = new G4MOptDeviceAction(namedetector.str() + "/" + nameCasing + namePixelL, detectorId, pixelLId, theEvent);
			sdMan->AddNewDetector(PixelTopL);
			logicPixelL->SetSensitiveDetector(PixelTopL);

			G4MOptDeviceAction* const PixelTopR = new G4MOptDeviceAction(namedetector.str() + "/" + nameCasing + namePixelR, detectorId, pixelRId, theEvent);
			sdMan->AddNewDetector(PixelTopR);
			logicPixelR->SetSensitiveDetector(PixelTopR);

		} // end loop of top panel

		// bars of the bottom panel
		for (G4int bIt=0; bIt<nBars; ++bIt) {

			// bar position (w.r.t casing)
			G4String gridName = "Y";
			int barId = bIt+1+nBars;
			G4double fBarPosX = bIt*(fBarWidth + 2*fCoatingThickness);//0 * CLHEP::mm;
			G4double fBarPosY =  - 0.5*fBarWidth;//0 * CLHEP::mm;
			G4double fBarPosZ = -1*(0.5*fBarThickness * 2 + fCasingThickness) / 2;
			//-fCasingSizeZ/2 - 2*fCoatingThickness
			G4ThreeVector barPosition = G4ThreeVector(fBarPosX-fHalfWidth, fBarPosY, fBarPosZ);

			// fiber position (w.r.t scintillator bar)
			G4double fFiberPosX = -1*GetFiberXCoordinate(bIt, fBarLength, fFiberLength);
			G4double fFiberPosY = 0 * CLHEP::mm;
			G4double fFiberPosZ = -0.5*fBarThickness + fCoatingThickness + fFiberRadius + fCladdingThickness;
			G4ThreeVector fiberPosition = G4ThreeVector(fFiberPosX, fFiberPosY, fFiberPosZ);

			// PMT position (w.r.t fiber)
			G4double fPMTPosX = 0 * CLHEP::mm;
			G4double fPMTPosY = 0 * CLHEP::mm;
			G4double fPMTPosZ = 0.5*fFiberLength + 0.5*fPMTSizeZ;

			// PMT indexing
			G4int pixelLId = 100*panelId + barId + nBars;   // (125...136, 225...236, 325...336)
			G4int pixelRId = 100*panelId + barId + 2*nBars; // (137...148, 237...248, 337...348)

			G4String nameCoating = "BarCoating_"+gridName+"_"+to_string(barId);
			G4String nameScinBar = "BarScin_"+gridName+"_"+to_string(barId);
			G4String nameClad2 = "FiberClad2_"+gridName+"_"+to_string(barId);
			G4String nameClad1 = "FiberClad1_"+gridName+"_"+to_string(barId);
			G4String nameFiber = "Fiber_"+gridName+"_"+to_string(barId);
			G4String namePixelL = "PMT_"+gridName+"_"+to_string(pixelLId);
			G4String namePixelR = "PMT_"+gridName+"_"+to_string(pixelRId);
			// register PMTs in the detector class
			detector.MakeOptDevice(pixelLId, OptDevice::ePMT);
			detector.MakeOptDevice(pixelRId, OptDevice::ePMT);

			// logical volumes
			logicCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, nameCoating, 0, 0, 0);
			logicScinBar  = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, nameScinBar, 0, 0, 0);
			logicClad2 = new G4LogicalVolume(solidClad2, Materials().FPethylene, nameClad2, 0, 0, 0);
			logicClad2->SetVisAttributes(green);
			logicClad1 = new G4LogicalVolume(solidClad1, Materials().Pethylene, nameClad1, 0, 0, 0);
			logicClad1->SetVisAttributes(green); 
			logicFiber = new G4LogicalVolume(solidFiber, Materials().PMMA, nameFiber, 0, 0, 0);
			logicFiber->SetVisAttributes(green);
			logicPixelL = new G4LogicalVolume(solidPixel, Materials().Pyrex, namePixelL, 0, 0, 0);
			logicPixelL->SetVisAttributes(blue);
			logicPixelR = new G4LogicalVolume(solidPixel, Materials().Pyrex, namePixelR, 0, 0, 0);
			logicPixelR->SetVisAttributes(blue);

			// physical volumes		
			physCoating = new G4PVPlacement(rotationBot, barPosition, nameCoating, logicCoating, physCasing, false, barId, fCheckOverlaps);
			physScinBar = new G4PVPlacement(0, G4ThreeVector(), nameScinBar, logicScinBar, physCoating, false, barId, fCheckOverlaps);
			physClad2 = new G4PVPlacement(rotationFiber, fiberPosition, nameClad2, logicClad2, physScinBar, false, barId, fCheckOverlaps);
			physClad1 = new G4PVPlacement(0, G4ThreeVector(), nameClad1, logicClad1, physClad2, false, barId, fCheckOverlaps);
			physFiber = new G4PVPlacement(0, G4ThreeVector(), nameFiber, logicFiber, physClad1, false, barId, fCheckOverlaps);
			new G4PVPlacement(0, G4ThreeVector(fPMTPosX, fPMTPosY, fPMTPosZ), namePixelL, logicPixelL, physFiber, false, pixelLId, fCheckOverlaps);
			new G4PVPlacement(0, G4ThreeVector(fPMTPosX, fPMTPosY, -1*fPMTPosZ), namePixelR, logicPixelR, physFiber, false, pixelRId, fCheckOverlaps);

			// registration of pixels as Sensitive Detectors
			G4MOptDeviceAction* const PixelBotL = new G4MOptDeviceAction(namedetector.str() + "/" + nameCasing + namePixelL, detectorId, pixelLId, theEvent);
			sdMan->AddNewDetector(PixelBotL);
			logicPixelL->SetSensitiveDetector(PixelBotL);

			G4MOptDeviceAction* const PixelBotR = new G4MOptDeviceAction(namedetector.str() + "/" + nameCasing + namePixelR, detectorId, pixelRId, theEvent);
			sdMan->AddNewDetector(PixelBotR);
			logicPixelR->SetSensitiveDetector(PixelBotR);

		} // end loop bottom panel
	
	} // end loop panels

}

G4double
Mudulus::GetFiberXCoordinate(G4int barId, G4double barLength, G4double fiberLength)
{
	G4double pos = 0;
	if (barId == 0)
		pos = -450;
	else if (barId == 1)
		pos = -500;
	else if (barId == 2)
		pos = -550;
	else if (barId == 3)
		pos = -600;
	else if (barId == 4)
		pos = -650;
	else if (barId == 5)
		pos = -700;
	else if (barId == 6)
		pos = -750;
	else if (barId == 7)
		pos = -800;
	else if (barId == 8)
		pos = -850;
	else if (barId == 9)
		pos = -900;
	else if (barId == 10)
		pos = -950;
	else if (barId == 11)
		pos = -1000;

	return (pos * CLHEP::mm - 0.5*barLength + 0.5*fiberLength);
}
