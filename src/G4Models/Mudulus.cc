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
Mudulus::BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOverlaps)
{

	G4SDManager* const sdMan = G4SDManager::GetSDMpointer();
	G4ThreeVector  detectorPos = Geometry::ToG4Vector(detector.GetDetectorPosition(), 1.);
	int detectorId = detector.GetId();
	int nBars = detector.GetNBars();

	// detector properties
	fCasingThickness = detector.GetCasingThickness() * mm;

	// scintillator bar properties
	fBarWidth  = detector.GetBarWidth() * mm;
	fBarLength = detector.GetBarLength() * mm; 
	fBarThickness = detector.GetBarThickness() * mm;
	fCoatingThickness = detector.GetBarCoatingThickness() * mm;
	fHalfWidth = 0.5*fBarWidth*nBars;

	// fiber properties
	fCladdingThickness = detector.GetCladdingThickness() * mm;
	fFiberRadius = detector.GetFiberRadius() * mm;

	OptDevice pmt = detector.GetOptDevice(OptDevice::ePMT);
	fPMTSizeX = pmt.GetLength() * mm;
	fPMTSizeY = pmt.GetWidth() * mm;
	fPMTSizeZ = pmt.GetThickness() * mm;
	
	// define a enclosure volume that contains the components of the detector
	fCasingSizeX = fBarLength + fCasingThickness;
	fCasingSizeY = fBarLength + fCasingThickness;
	fCasingSizeZ = 0.5*fBarThickness * 2 + fCasingThickness; // (x2) number of panels 
	
	ostringstream namedetector;
	namedetector.str("");
	namedetector << "Mudulus";
	std::cout << "G4Models::Mudulus: Building detector " << namedetector.str();
	std::cout << " (" << detectorId << ")";
	std::cout << " with " << pmt.GetName() << ". " << std::endl;

	/****************************
		
		Geant4 Volume construction
	
	****************************/ 

	// Casing
	solidCasing = new G4Box("Casing", fCasingSizeX, fCasingSizeY, fCasingSizeZ);
	logicCasing = new G4LogicalVolume(solidCasing, Materials().Air, "Casing", 0, 0, 0);
	physCasing  = new G4PVPlacement(nullptr, detectorPos, logicCasing, "Casing", logMother, false, detectorId, fCheckOverlaps);
	// Bars: Coating + Scintillator bar
	solidCoating  	= new G4Box("BarCoating", 0.5*fBarLength + fCoatingThickness, 0.5*fBarWidth + fCoatingThickness, 0.5*fBarThickness + fCoatingThickness);
	solidScinBar   	= new G4Box("BarScin", 0.5*fBarLength, 0.5*fBarWidth, 0.5*fBarThickness);
	// Fiber: Cladding (external & internal) + fiber core
	solidClad2  = new G4Tubs("Clad2", 0, fFiberRadius, 0.5*fBarLength + 0.5*fFiberExtra, 0, 360*deg);
	solidClad1  = new G4Tubs("Clad1", 0, fFiberRadius - fCladdingThickness, 0.5*fBarLength + 0.5*fFiberExtra, 0, 360*deg);
	solidFiber  = new G4Tubs("Fiber", 0, fFiberRadius - 2*fCladdingThickness, 0.5*fBarLength + 0.5*fFiberExtra, 0, 360*deg);
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

	// definitions for fibers and PMT placements
	G4double fFiberPosZ = -0.5*fBarThickness + fCoatingThickness + fFiberRadius + fCladdingThickness;
	G4double fPMTPosZ = 0.5*fBarLength + 0.5*fFiberExtra + 0.5*fPMTSizeZ;

	// bars of the top panel
	for (G4int i=0; i<nBars; ++i) {

		// definitions for bar placement
		G4String panelId = "X";
		int barId = i+1;
		G4double yPos = i*(fBarWidth + 2*fCoatingThickness);
		// shift each bar position by 1/2 of the panel width
		yPos -= fHalfWidth;
		G4String nameCoating = "BarCoating_"+panelId+"_"+to_string(barId);
		G4String nameScinBar = "BarScin_"+panelId+"_"+to_string(barId);
		G4String nameClad2 = "FiberClad2_"+panelId+"_"+to_string(barId);
		G4String nameClad1 = "FiberClad1_"+panelId+"_"+to_string(barId);
		G4String nameFiber = "Fiber_"+panelId+"_"+to_string(barId);
		G4String namePixelL = "PMT_"+panelId+"_"+to_string(barId)+"_left";
		G4String namePixelR = "PMT_"+panelId+"_"+to_string(barId)+"_right";
		// register PMTs in the detector class
		detector.MakeOptDevice(-1*barId, OptDevice::ePMT);
		detector.MakeOptDevice(barId, OptDevice::ePMT);
		

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
		physCoating  = new G4PVPlacement(nullptr, G4ThreeVector(0, yPos, fCasingSizeZ/2), logicCoating, 
			nameCoating, logicCasing, false, barId, fCheckOverlaps);
		physScinBar   = new G4PVPlacement(nullptr, G4ThreeVector(), logicScinBar, 
			nameScinBar, logicCoating, false, barId, fCheckOverlaps);
		physClad2 = new G4PVPlacement(rotationFiber, G4ThreeVector(0, 0, fFiberPosZ), logicClad2, 
			nameClad2, logicScinBar, true, barId, fCheckOverlaps);
		physClad1 = new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1, 
			nameClad1, logicClad2, false, barId, fCheckOverlaps);
		physFiber = new G4PVPlacement(nullptr, G4ThreeVector(), logicFiber, 
			nameFiber, logicClad1, false, barId, fCheckOverlaps); 
		


		physPixelL  = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fPMTPosZ), logicPixelL,
			namePixelL, logicFiber, false, -1*barId, fCheckOverlaps);
		physPixelR  = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, -1*fPMTPosZ), logicPixelR,
			namePixelR, logicFiber, false, barId, fCheckOverlaps);

		// registration of PMTs as Sensitive Detectors
		G4MOptDeviceAction* const PixelTopL = new G4MOptDeviceAction(namedetector.str() + "/" + namePixelL, detectorId, -1*barId, theEvent);
		sdMan->AddNewDetector(PixelTopL);
		logicPixelL->SetSensitiveDetector(PixelTopL);

		G4MOptDeviceAction* const PixelTopR = new G4MOptDeviceAction(namedetector.str() + "/" + namePixelR, detectorId, barId, theEvent);
		sdMan->AddNewDetector(PixelTopR);
		logicPixelR->SetSensitiveDetector(PixelTopR);

		
  }

	// bars of the bottom panel
	for (G4int i=0; i<nBars; ++i) {

		// definitions for bar placement
		G4String panelId = "Y";
		int barId = i+nBars+1;
		G4double xPos = i*(fBarWidth + 2*fCoatingThickness);
		xPos -= fHalfWidth;
		string nameCoating = "BarCoating_"+panelId+"_"+to_string(barId);
		string nameScinBar = "BarScin_"+panelId+"_"+to_string(barId);
		string nameClad2 = "FiberClad2_"+panelId+"_"+to_string(barId);
		string nameClad1 = "FiberClad1_"+panelId+"_"+to_string(barId);
		string nameFiber = "Fiber_"+panelId+"_"+to_string(barId);
		G4String namePixelL = "PMT_"+panelId+"_"+to_string(barId)+"_left";
		G4String namePixelR = "PMT_"+panelId+"_"+to_string(barId)+"_right";
		// register PMTs in the detector class
		detector.MakeOptDevice(-1*barId, OptDevice::ePMT);
		detector.MakeOptDevice(barId, OptDevice::ePMT);

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
		physCoating  = new G4PVPlacement(rotationBot, G4ThreeVector(xPos, 0, -fCasingSizeZ/2 - 2*fCoatingThickness), logicCoating, 
			nameCoating, logicCasing, false, barId, fCheckOverlaps);
		physScinBar   = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logicScinBar,
			nameScinBar, logicCoating, false, barId, fCheckOverlaps);
		physClad2 = new G4PVPlacement(rotationFiber, G4ThreeVector(0, 0, fFiberPosZ), logicClad2, 
			nameClad2, logicScinBar, true, barId, fCheckOverlaps);
		physClad1 = new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1, 
			nameClad1, logicClad2, false, barId, fCheckOverlaps);
		physFiber = new G4PVPlacement(nullptr, G4ThreeVector(), logicFiber, 
			nameFiber, logicClad1, false, barId, fCheckOverlaps); 
		physPixelL  = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fPMTPosZ), logicPixelL, 
				namePixelL, logicFiber, false, -1*barId, fCheckOverlaps);
		physPixelR  = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, -1*fPMTPosZ), logicPixelR, 
				namePixelR, logicFiber, false, barId, fCheckOverlaps);

		// registration of pixels as Sensitive Detectors
		G4MOptDeviceAction* const PixelBotL = new G4MOptDeviceAction(namedetector.str() + "/" + namePixelL, detectorId, -1*barId, theEvent);
		sdMan->AddNewDetector(PixelBotL);
		logicPixelL->SetSensitiveDetector(PixelBotL);

		G4MOptDeviceAction* const PixelBotR = new G4MOptDeviceAction(namedetector.str() + "/" + namePixelR, detectorId, barId, theEvent);
		sdMan->AddNewDetector(PixelBotR);
		logicPixelR->SetSensitiveDetector(PixelBotR);

	}

	
}
