#include "Hodoscope.h"
#include "Geometry.h"
#include "Materials.h"
#include "G4MScintillatorBarAction.h"
#include "G4MOptDeviceAction.h"

#include "G4SDManager.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"

#include <iostream>
using namespace std;

void
Hodoscope::BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOverlaps)
{


	// solids
	G4Box* solidEnclosure = nullptr;
	G4Box* solidCasing = nullptr;
	G4Box* solidCoating = nullptr;
	G4Box* solidScinBar = nullptr;

	G4Tubs* solidClad2 = nullptr;
	G4Tubs* solidClad1 = nullptr;
	G4Tubs* solidFiber = nullptr;

	G4Box* solidPixel = nullptr;

	// logical volumes
	G4LogicalVolume* logEnclosure = nullptr;
	G4LogicalVolume* logCasing = nullptr;
	G4LogicalVolume* logCoating = nullptr;
	G4LogicalVolume* logScinBar = nullptr;

	G4LogicalVolume* logClad2 = nullptr;
	G4LogicalVolume* logClad1 = nullptr;
	G4LogicalVolume* logFiber = nullptr;

	G4LogicalVolume* logPixel = nullptr;

	G4SDManager* const sdMan = G4SDManager::GetSDMpointer();

	// number of scintillator bars
	int nBars = detector.GetNBars();
	
	// Scintillator bars
	double barWidth = detector.GetBarWidth();
	double barLength = detector.GetBarLength();
	double barThickness = detector.GetBarThickness();
	double barCoatingThickness = detector.GetBarCoatingThickness();
	double detectorWidth = nBars * (barWidth + barCoatingThickness);
	double halfWidth = 0.5 * nBars * (barWidth + barCoatingThickness);

	// optical fibers
	double claddingThickness = detector.GetCladdingThickness();
	double fiberRadius = detector.GetFiberRadius();

	// optical device
	OptDevice pixel = detector.GetOptDevice(OptDevice::eMChPMT);
	double pixelSizeX = pixel.GetLength();
	double pixelSizeY = pixel.GetWidth();
	double pixelSizeZ = pixel.GetThickness();

	// detector
	G4ThreeVector detectorPos = Geometry::ToG4Vector(detector.GetDetectorPosition(), 1.);
	int detectorId = detector.GetId();
	ostringstream namedetector;
	namedetector.str("");
	namedetector << "Hodoscope_"+to_string(detectorId);
	cout << "[INFO] Hodoscope::BuildDetector: Building detector " << namedetector.str();
	cout << " (ID = " << detectorId << ")";
	cout << " with " << nBars << " scintillator bars " << endl;

	// determine casing size
	double maxLength;
	maxLength = (barLength > detectorWidth ? barLength : detectorWidth);

	const double fCasingThickness = 2 * CLHEP::mm;
	const double fCasingSizeX = maxLength + fCasingThickness;
	const double fCasingSizeY = maxLength + fCasingThickness;
	const double fCasingSizeZ = barThickness * 4;

	const double fEnclosureSizeX = fCasingSizeX;
	const double fEnclosureSizeY = fCasingSizeY;
	const double fEnclosureSizeZ = fCasingSizeZ;

	// Detector enclosure
	solidEnclosure = new G4Box("Enclosure", 0.5*fEnclosureSizeX, 0.5*fEnclosureSizeY, 0.5*fEnclosureSizeZ);
	// Panel casing
	solidCasing = new G4Box("Casing", 0.5*fCasingSizeX, 0.5*fCasingSizeY, 0.5*fCasingSizeZ);
	// Scintillator bars
	solidCoating = new G4Box("BarCoating", 0.5*barLength + barCoatingThickness,
																				 0.5*barWidth + barCoatingThickness,
																				 0.5*barThickness + barCoatingThickness);
	solidScinBar = new G4Box("BarScin", 0.5*barLength,
																			0.5*barWidth,
																			0.5*barThickness);
	// Optical Fibers
	solidClad2  = new G4Tubs("Clad2", 0, fiberRadius, barLength/2-2*barCoatingThickness-pixelSizeZ, 0, 360*deg);
	solidClad1  = new G4Tubs("Clad1", 0, fiberRadius - claddingThickness, barLength/2-2*barCoatingThickness-pixelSizeZ, 0, 360*deg);
	solidFiber  = new G4Tubs("Fiber", 0, fiberRadius - 2*claddingThickness, barLength/2-2*barCoatingThickness-pixelSizeZ, 0, 360*deg);
	// Pixel
	solidPixel   = new G4Box("Pixel", pixelSizeX, pixelSizeY, pixelSizeZ);
	

	// Defne BarCoating as LogicalSkingSurface
	new G4LogicalSkinSurface("BarCoating", logCoating, Materials().ScinOptSurf);

	// useful rotation matrix for bars orientation
	G4RotationMatrix* rotationEnclosure = new G4RotationMatrix();
	G4RotationMatrix* rotationFiber = new G4RotationMatrix();
	G4RotationMatrix* rotationBot = new G4RotationMatrix();
	double alpha = detector.GetRotationAngle();
	cout << "[INFO] Hodoscope::BuildDetector: Rotation Angle = " << alpha << endl;
	rotationEnclosure->rotateX(alpha);
	rotationFiber->rotateY(M_PI/2);
	rotationBot->rotateZ(M_PI/2);

	logEnclosure = new G4LogicalVolume(solidEnclosure, Materials().Air, "Enclosure", 0, 0, 0);
	new G4PVPlacement(rotationEnclosure, G4ThreeVector(detectorPos.getX(), detectorPos.getY(), detectorPos.getZ()), logEnclosure, "Enclosure", logMother, false, detectorId, fCheckOverlaps);

	// color definitions for visualization
	G4VisAttributes green(G4Colour::Green());
	G4VisAttributes blue(G4Colour::Blue());
	G4VisAttributes black(G4Colour::Black());

	int panelId = detectorId+1;

	string nameCasing = "Casing_"+to_string(panelId);

	logCasing = new G4LogicalVolume(solidCasing, Materials().Air, nameCasing, 0, 0, 0);
	new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logCasing, nameCasing, logEnclosure, false, panelId, fCheckOverlaps);

	double fFiberPosX = 0.5*barLength - 2*barCoatingThickness;
	double fFiberPosZ = fiberRadius + claddingThickness;

	// bars of the top panel
	for (int bIt=0; bIt<nBars; ++bIt) {

		string gridName = "X";
		int barId = bIt+1;
		int barId2 = 100*panelId + barId;

		double fBarPosX = -0.5*barWidth;
		double fBarPosY = bIt * (barWidth + 2*barCoatingThickness);
		// center bar at top of the casing
		double fBarPosZ = 0.5*(0.5*barThickness*2 + fCasingThickness);
		
		G4ThreeVector barPosition = G4ThreeVector(fBarPosX+0.5*barWidth, fBarPosY-halfWidth+0.5*barWidth, fBarPosZ);
		
		string nameCoating = "BarCoating_"+gridName;
		string nameScinBar = "BarScin_"+gridName;
		string nameClad2 = "FiberClad2_"+gridName;
		string nameClad1 = "FiberClad1_"+gridName;
		string nameFiber = "Fiber_"+gridName;
		string namePixel = "Pixel_"+gridName+to_string(barId2);
		// register pixel in the detector class
		detector.MakeOptDevice(barId, OptDevice::eMChPMT);

		// logical volumes
		logCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, nameCoating, 0, 0, 0);
		logScinBar = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, nameScinBar, 0, 0, 0);
		logClad2 = new G4LogicalVolume(solidClad2, Materials().FPethylene, nameClad2, 0, 0, 0);
		logClad2->SetVisAttributes(green);
		logClad1 = new G4LogicalVolume(solidClad1, Materials().Pethylene, nameClad1, 0, 0, 0);
		logClad1->SetVisAttributes(green);
		logFiber = new G4LogicalVolume(solidFiber, Materials().PMMA, nameFiber, 0, 0, 0);
		logFiber->SetVisAttributes(green);
		logPixel = new G4LogicalVolume(solidPixel, Materials().Pyrex, namePixel, 0, 0, 0);
		logPixel->SetVisAttributes(blue);

		// physical volumes
		new G4PVPlacement(nullptr, barPosition, logCoating, nameCoating, logCasing, false, barId2, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(), logScinBar, nameScinBar, logCoating, false, barId2, fCheckOverlaps);
		// register scintillator bar as Sensitive Detector to store hits
		G4MScintillatorBarAction* const scinBarSD = new G4MScintillatorBarAction(namedetector.str() + "/" + nameScinBar+"_"+to_string(barId), detectorId, barId, theEvent);
		sdMan->AddNewDetector(scinBarSD);
		logScinBar->SetSensitiveDetector(scinBarSD);

		new G4PVPlacement(rotationFiber, G4ThreeVector(0., 0., fFiberPosZ), logClad2, nameClad2, logScinBar, true, barId2, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logClad1, nameClad1, logClad2, false, barId2, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logFiber, nameFiber, logClad1, false, barId2, fCheckOverlaps);

		new G4PVPlacement(rotationFiber, G4ThreeVector(fFiberPosX, 0, fFiberPosZ), logPixel, namePixel, logScinBar, false, barId2, fCheckOverlaps);

		// registration of pixel as a Sensitive Volume
		G4MOptDeviceAction* const pixelTopSD = new G4MOptDeviceAction(namedetector.str() + "/" + namePixel, detectorId, barId, theEvent);
		sdMan->AddNewDetector(pixelTopSD);
		logPixel->SetSensitiveDetector(pixelTopSD);

	} // end loop of bars of the top panel

	// bars of the bottom panel
	for (int bIt=0; bIt<nBars; ++bIt) {

		string gridName = "Y";
		int barId = bIt+1+nBars;
		int barId2 = 100*panelId + barId;

		double fBarPosX = bIt * (barWidth + 2*barCoatingThickness);
		double fBarPosY = -0.5*barWidth;
		// center bar at top of the casing
		double fBarPosZ = -0.5*(0.5*barThickness*2 + fCasingThickness);

		G4ThreeVector barPosition = G4ThreeVector(fBarPosX-halfWidth+0.5*barWidth, fBarPosY+0.5*barWidth, fBarPosZ);

		string nameCoating = "BarCoating_"+gridName;
		string nameScinBar = "BarScin_"+gridName;
		string nameClad2 = "FiberClad2_"+gridName;
		string nameClad1 = "FiberClad1_"+gridName;
		string nameFiber = "Fiber_"+gridName;
		string namePixel = "Pixel_"+gridName+to_string(barId2);
		// register pixel in the detector class
		detector.MakeOptDevice(barId, OptDevice::eMChPMT);

		// logical volumes
		logCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, nameCoating, 0, 0, 0);
		logScinBar = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, nameScinBar, 0, 0, 0);
		logClad2 = new G4LogicalVolume(solidClad2, Materials().FPethylene, nameClad2, 0, 0, 0);
		logClad2->SetVisAttributes(green);
		logClad1 = new G4LogicalVolume(solidClad1, Materials().Pethylene, nameClad1, 0, 0, 0);
		logClad1->SetVisAttributes(green);
		logFiber = new G4LogicalVolume(solidFiber, Materials().PMMA, nameFiber, 0, 0, 0);
		logFiber->SetVisAttributes(green);
		logPixel = new G4LogicalVolume(solidPixel, Materials().Pyrex, namePixel, 0, 0, 0);
		logPixel->SetVisAttributes(blue);

		// physical volumes
		new G4PVPlacement(rotationBot, barPosition, logCoating, nameCoating, logCasing, false, barId2, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(), logScinBar, nameScinBar, logCoating, false, barId2, fCheckOverlaps);
		
		G4MScintillatorBarAction* const scinBarSD = new G4MScintillatorBarAction(namedetector.str() + "/" + nameScinBar+"_"+to_string(barId), detectorId, barId, theEvent);
		sdMan->AddNewDetector(scinBarSD);
		logScinBar->SetSensitiveDetector(scinBarSD);
		// register scintillator bar as Sensitive Detector to store hits
		new G4PVPlacement(rotationFiber, G4ThreeVector(0, 0, fFiberPosZ), logClad2, nameClad2, logScinBar, true, barId2, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logClad1, nameClad1, logClad2, false, barId2, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logFiber, nameFiber, logClad1, false, barId2, fCheckOverlaps);

		new G4PVPlacement(rotationFiber, G4ThreeVector(fFiberPosX, 0, fFiberPosZ), logPixel, namePixel, logScinBar, false, barId2, fCheckOverlaps);

		// registration of pixel as a Sensitive Volume
		G4MOptDeviceAction* const pixelBotSD = new G4MOptDeviceAction(namedetector.str() + "/" + namePixel, detectorId, barId, theEvent);
		sdMan->AddNewDetector(pixelBotSD);
		logPixel->SetSensitiveDetector(pixelBotSD);


	} // end loop of batrs of the bottom panel

}