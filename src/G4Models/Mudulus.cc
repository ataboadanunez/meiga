#include "Mudulus.h"
#include "Geometry.h"
#include "G4MOptDeviceAction.h"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include <iostream>

using namespace std;

Mudulus::Mudulus()
{

}

Mudulus::~Mudulus()
{

}

void 
Mudulus::BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4int fBarsPanel, G4bool fCheckOverlaps)
{

	G4SDManager* const sdMan = G4SDManager::GetSDMpointer();

	// detector position
	G4ThreeVector  detectorPos = Geometry::ToG4Vector(detector.GetDetectorPosition(), 1.);
	int detectorId = detector.GetId();
	int nBars = detector.GetNBars();
	// detector properties
	
	std::cout << "[DEBUG] Mudulus::BuildDetector: Creating Detector Mudulus with " << nBars << " bars." << std::endl;

	// scintillator bar properties
	fBarWidth  = detector.GetBarWidth() * mm;
	fBarLength = detector.GetBarLength(); 
	fBarThickness = detector.GetBarThickness() * mm;
	fCoatingThickness = detector.GetBarCoatingThickness() * mm;
	G4double fHalfWidth = 0.5*fBarWidth*nBars;

	// fiber properties
	fCladdingThickness = detector.GetCladdingThickness() * mm;
	fFiberRadius = detector.GetFiberRadius() * mm;

	OptDevice& pmt = detector.GetOptDevice();
	fPixelSizeZ = pmt.GetWidth() * mm;
	fPixelSizeX = fPixelSizeY = pmt.GetLength() * mm;

	// define a enclosure volume that contains the components of the detector
	// this is just to ease the detector construction...not a proper volume
	ostringstream namedetector;
	namedetector.str("");
	namedetector << "Casing" << '_' << detectorId;
	fCasingSizeX = fBarLength;
	fCasingSizeY = fBarLength;
	fCasingSizeZ = 0.5*fBarThickness * 2; // (x2) number of panels 
	fCasingThickness = detector.GetCasingThickness() * mm;
	
	// Casing
	solidCasing = new G4Box("Casing", fCasingSizeX+fCasingThickness,fCasingSizeY+fCasingThickness, fCasingSizeZ+fCasingThickness);
	logicCasing = new G4LogicalVolume(solidCasing, Materials().Air, "Casing", 0, 0, 0);
	physCasing  = new G4PVPlacement(nullptr, detectorPos, logicCasing, "Casing", logMother, false, detectorId, fCheckOverlaps);
	// Bars: Coating + Scintillator bar
	solidCoating  	= new G4Box("BarCoating", 0.5*fBarLength + fCoatingThickness, 0.5*fBarWidth + fCoatingThickness, 0.5*fBarThickness + fCoatingThickness);
	solidScinBar   	= new G4Box("BarScin", 0.5*fBarLength, 0.5*fBarWidth, 0.5*fBarThickness);
	// Fiber: Cladding (external & internal) + fiber core
	// solidClad2  = new G4Tubs("Clad2", 0, fFiberRadius, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	// solidClad1  = new G4Tubs("Clad1", 0, fFiberRadius - fCladdingThickness, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	// solidFiber  = new G4Tubs("Fiber", 0, fFiberRadius - 2*fCladdingThickness, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	solidClad2  = new G4Tubs("Clad2", 0, fFiberRadius, 0.5*fBarLength + 0.5*fFiberExtra, 0, 360*deg);
	solidClad1  = new G4Tubs("Clad1", 0, fFiberRadius - fCladdingThickness, 0.5*fBarLength + 0.5*fFiberExtra, 0, 360*deg);
	solidFiber  = new G4Tubs("Fiber", 0, fFiberRadius - 2*fCladdingThickness, 0.5*fBarLength + 0.5*fFiberExtra, 0, 360*deg);

	solidPixel   = new G4Box("PMT", fPixelSizeX, fPixelSizeY, fPixelSizeZ);
	
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

	G4double fFiberPosZ = -fBarThickness/2 + fCoatingThickness + fFiberRadius + fCladdingThickness;
	G4double fPixelPosZ = 0.5*fBarLength + 0.5*fFiberExtra - 0.5*fPixelSizeZ;

	// bars of the top panel
	for (G4int i=0; i<nBars; ++i) {

		/* 
			place bars in the top panel along y-axis
		*/

		string panelId = "X";
		unsigned int barId = i;
		G4double yPos = i*(fBarWidth + 2*fCoatingThickness);
		// shift each bar position by 1/2 of the panel width
		yPos -= fHalfWidth;
		string nameCoating = "BarCoating_"+panelId+"_"+to_string(barId);
		string nameScinBar = "BarScin_"+panelId+"_"+to_string(barId);
		string nameClad2 = "FiberClad2_"+panelId+"_"+to_string(barId);
		string nameClad1 = "FiberClad1_"+panelId+"_"+to_string(barId);
		string nameFiber = "Fiber_"+panelId+"_"+to_string(barId);
		string namePixelL = "Pixel_left_"+panelId+"_"+to_string(barId);
		string namePixelR = "Pixel_right_"+panelId+"_"+to_string(barId);

#warning "dont use bar ID for PMTs. 2 PMTs (with different ID) per bar!"
		detector.MakeOptDevice(barId, OptDevice::ePMT);

		// logic volumes for scintillator bars
		logicCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, nameCoating, 0, 0, 0);
		logicScinBar  = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, nameScinBar, 0, 0, 0);
		// logic volumes for WLS fibers
		logicClad2 = new G4LogicalVolume(solidClad2, Materials().FPethylene, nameClad2, 0, 0, 0);
		logicClad2->SetVisAttributes(green);
		logicClad1 = new G4LogicalVolume(solidClad1, Materials().Pethylene, nameClad1, 0, 0, 0);
		logicClad1->SetVisAttributes(green); 
		logicFiber = new G4LogicalVolume(solidFiber, Materials().PMMA, nameFiber, 0, 0, 0);
		logicFiber->SetVisAttributes(green); 
		// logic volumes for pixels of the optical device
		logicPixelL = new G4LogicalVolume(solidPixel, Materials().Pyrex, namePixelL, 0, 0, 0);
		logicPixelL->SetVisAttributes(blue); 
		logicPixelR = new G4LogicalVolume(solidPixel, Materials().Pyrex, namePixelR, 0, 0, 0);
		logicPixelR->SetVisAttributes(blue);

		// phys volumes for bars
		physCoating  = new G4PVPlacement(nullptr, G4ThreeVector(0, yPos, fCasingSizeZ/2), logicCoating, 
			nameCoating, logicCasing, false, barId, fCheckOverlaps);
		physScinBar   = new G4PVPlacement(nullptr, G4ThreeVector(), logicScinBar, 
			nameScinBar, logicCoating, false, barId, fCheckOverlaps);
		// phys volumes for fibers
		physClad2 = new G4PVPlacement(rotationFiber, G4ThreeVector(0, 0, fFiberPosZ), logicClad2, 
			nameClad2, logicScinBar, true, barId, fCheckOverlaps);
		physClad1 = new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1, 
			nameClad1, logicClad2, false, barId, fCheckOverlaps);
		physFiber = new G4PVPlacement(nullptr, G4ThreeVector(), logicFiber, 
			nameFiber, logicClad1, false, barId, fCheckOverlaps); 
		// phys volumes for pixels of the optical device
		physPixelL  = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fPixelPosZ), logicPixelL,
			namePixelL, logicFiber, false, barId, fCheckOverlaps);
		physPixelR  = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, -1*fPixelPosZ), logicPixelR,
			namePixelR, logicFiber, false, barId, fCheckOverlaps);

		// registration of pixels as Sensitive Detectors
		G4MOptDeviceAction* const PixelTopL = new G4MOptDeviceAction("/Mudulus/" + namedetector.str() + "/" + namePixelL, detectorId, barId, theEvent);
		sdMan->AddNewDetector(PixelTopL);
		logicPixelL->SetSensitiveDetector(PixelTopL);

		G4MOptDeviceAction* const PixelTopR = new G4MOptDeviceAction("/Mudulus/" + namedetector.str() + "/" + namePixelR, detectorId, barId, theEvent);
		sdMan->AddNewDetector(PixelTopR);
		logicPixelR->SetSensitiveDetector(PixelTopR);

		
  }

	// bars of the bottom panel
	
	for (G4int i=0; i<nBars; ++i) {

		/* 
			place bars in the bottom panel along x-axis
		*/

		string panelId = "Y";
		unsigned int barId = i+nBars;
		G4double xPos = i*(fBarWidth + 2*fCoatingThickness);
		xPos -= fHalfWidth;
		string nameCoating = "BarCoating_"+panelId+"_"+to_string(i);
		string nameScinBar = "BarScin_"+panelId+"_"+to_string(i);
		string nameClad2 = "FiberClad2_"+panelId+"_"+to_string(i);
		string nameClad1 = "FiberClad1_"+panelId+"_"+to_string(i);
		string nameFiber = "Fiber_"+panelId+"_"+to_string(i);
		string namePixelL = "Pixel_left_"+panelId+"_"+to_string(barId);
		string namePixelR = "Pixel_right_"+panelId+"_"+to_string(barId);

#warning "dont use bar ID for PMTs. 2 PMTs (with different ID) per bar!"
		detector.MakeOptDevice(barId, OptDevice::ePMT);

		// logic volumes for scintillator bars
		logicCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, nameCoating, 0, 0, 0);
		logicScinBar  = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, nameScinBar, 0, 0, 0);
		// logic volumes for WLS fibers
		logicClad2 = new G4LogicalVolume(solidClad2, Materials().FPethylene, nameClad2, 0, 0, 0);
		logicClad2->SetVisAttributes(green);
		logicClad1 = new G4LogicalVolume(solidClad1, Materials().Pethylene, nameClad1, 0, 0, 0);
		logicClad1->SetVisAttributes(green); 
		logicFiber = new G4LogicalVolume(solidFiber, Materials().PMMA, nameFiber, 0, 0, 0);
		logicFiber->SetVisAttributes(green);
		// logic volumes for pixels of the optical device
		logicPixelL = new G4LogicalVolume(solidPixel, Materials().Pyrex, namePixelL, 0, 0, 0);
		logicPixelL->SetVisAttributes(blue);
		logicPixelR = new G4LogicalVolume(solidPixel, Materials().Pyrex, namePixelR, 0, 0, 0);
		logicPixelR->SetVisAttributes(blue);

		// phys volumes for bars		
		physCoating  = new G4PVPlacement(rotationBot, G4ThreeVector(xPos, 0, -fCasingSizeZ/2 - 2*fCoatingThickness), logicCoating, 
			nameCoating, logicCasing, false, barId, fCheckOverlaps);
		physScinBar   = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logicScinBar,
			nameScinBar, logicCoating, false, barId, fCheckOverlaps);
		// phys volumes for fibers
		physClad2 = new G4PVPlacement(rotationFiber, G4ThreeVector(0, 0, fFiberPosZ), logicClad2, 
			nameClad2, logicScinBar, true, barId, fCheckOverlaps);
		physClad1 = new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1, 
			nameClad1, logicClad2, false, barId, fCheckOverlaps);
		physFiber = new G4PVPlacement(nullptr, G4ThreeVector(), logicFiber, 
			nameFiber, logicClad1, false, barId, fCheckOverlaps); 
		// phys volumes for optical devices
		physPixelL  = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fPixelPosZ), logicPixelL, 
				namePixelL, logicFiber, false, barId, fCheckOverlaps);
		physPixelR  = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, -1*fPixelPosZ), logicPixelR, 
				namePixelR, logicFiber, false, barId, fCheckOverlaps);

		// registration of pixels as Sensitive Detectors
		G4MOptDeviceAction* const PixelBotL = new G4MOptDeviceAction("/Mudulus/" + namedetector.str() + "/" + namePixelL, detectorId, barId, theEvent);
		sdMan->AddNewDetector(PixelBotL);
		logicPixelL->SetSensitiveDetector(PixelBotL);

		G4MOptDeviceAction* const PixelBotR = new G4MOptDeviceAction("/Mudulus/" + namedetector.str() + "/" + namePixelR, detectorId, barId, theEvent);
		sdMan->AddNewDetector(PixelBotR);
		logicPixelR->SetSensitiveDetector(PixelBotR);

	}

	
}
