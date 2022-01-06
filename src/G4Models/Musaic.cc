#include "Musaic.h"
#include "Geometry.h"
#include "G4MOptDeviceAction.h"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include <iostream>

using namespace std;

Musaic::Musaic()
{

}

Musaic::~Musaic()
{

}

void 
Musaic::BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4int fBarsPanel, G4bool fCheckOverlaps)
{

	G4SDManager* const sdMan = G4SDManager::GetSDMpointer();
  auto pos = detector.GetDetectorPosition();
  auto  detectorPos = Geometry::ToG4Vector(pos, 1.);
	int detectorId = detector.GetId();
	int nBars = detector.GetNBars();
	
	// detector properties
	fCasingThickness = detector.GetCasingThickness() * mm;
	// scintillator bar properties
	fBarWidth  = detector.GetBarWidth() * mm;
	fBarLength = detector.GetBarLength() * mm;
	fBarThickness = detector.GetBarThickness() * mm;
	fCoatingThickness = detector.GetBarCoatingThickness() * mm;

	// fiber properties
	fCladdingThickness = detector.GetCladdingThickness() * mm;
	fFiberRadius = detector.GetFiberRadius() * mm;

	// Optical device properties
	OptDevice sipm = detector.GetOptDevice(OptDevice::eSiPM);
	fSiPMSizeZ = sipm.GetThickness() * mm;
	fSiPMSizeX = sipm.GetLength() * mm;
	fSiPMSizeY = sipm.GetWidth() * mm;

	// define a enclosure volume that contains the components of the detector
	fCasingSizeX = 0.5*fBarLength + fCasingThickness;
	fCasingSizeY = 0.5*fBarLength + fCasingThickness;
	fCasingSizeZ = 0.5*fBarThickness * 2 + fCasingThickness; // (x2 = number of panels)

	string namedetector = "Musaic";
	string newname; 
	newname = namedetector + '_' + to_string(detectorId) + '/';
	
	std::cout << "[DEBUG] G4Models::Musaic: Building detector " << namedetector << " with " << sipm.GetName() << ". " << std::endl;
	

	/****************************
		
		Geant4 Volume construction
	
	****************************/ 

	// Casing
	solidCasing = new G4Box("Casing", fCasingSizeX, fCasingSizeY, fCasingSizeZ);
	logicCasing = new G4LogicalVolume(solidCasing, Materials().Air, "Casing", 0, 0, 0);
	physCasing  = new G4PVPlacement(nullptr, detectorPos, logicCasing, "Casing", logMother, false, detectorId, fCheckOverlaps);
	// Bars: Coating + Scintillator bar
	solidCoating  	= new G4Box("BarCoating", fCoatingThickness + fBarLength/2, fCoatingThickness + fBarWidth/2, fCoatingThickness + fBarThickness/2);
	solidScinBar   	= new G4Box("BarScin", fBarLength/2, fBarWidth/2, fBarThickness/2);
	// Fiber: Cladding (external & internal) + fiber core
	solidClad2  = new G4Tubs("Clad2", 0, fFiberRadius, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	solidClad1  = new G4Tubs("Clad1", 0, fFiberRadius - fCladdingThickness, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	solidFiber  = new G4Tubs("Fiber", 0, fFiberRadius - 2*fCladdingThickness, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	// SiPM
	solidSiPM   = new G4Box("SiPM", fSiPMSizeX, fSiPMSizeY, fSiPMSizeZ);
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

	// definitions for fibers and SiPM placements

	G4double fPosTopX = 0;
	G4double fPosTopY = 0.5*fBarWidth + fCoatingThickness;
	G4double fPosTopZ = 0.5*fBarThickness + fCoatingThickness;
	// bottom panel is rotated in the XY axis
	G4double fPosBotX = fPosTopY;
	G4double fPosBotY = 0;
	G4double fPosBotZ = -fPosTopZ;
	// place fiber at the top of the bar (x-axis)
	// place SiPM at the end of the bar (z-axis)
	G4double fFiberTopPosX = fBarLength/2 - 2*fCoatingThickness;
	G4double fFiberTopPosZ = -fBarThickness/2 + fCoatingThickness + fFiberRadius + fCladdingThickness;
	G4double fFiberBotPosZ = fFiberTopPosZ;

	// bars of the top panel
	for (G4int i=0; i<nBars; ++i) {

		// definitions for bar placement
		unsigned int barId = i;
		G4int inc = pow(-1,i);//(1+2*i);
		G4double yp = inc*fPosTopY;
		
		// naming
		G4String nameCoating = "BarCoating_"+to_string(barId);
		G4String nameScinBar = "BarScin_"+to_string(barId);
		G4String nameClad2 = "FiberClad2_"+to_string(barId);
		G4String nameClad1 = "FiberClad1_"+to_string(barId);
		G4String nameFiber = "Fiber_"+to_string(barId);
		G4String nameSiPM = "SiPM_"+to_string(barId);
		// register SiPM in the detector class
		detector.MakeOptDevice(barId, OptDevice::eSiPM);

		// logical volumes
		logicCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, nameCoating, 0, 0, 0);
		logicScinBar  = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, nameScinBar, 0, 0, 0);
		logicClad2 = new G4LogicalVolume(solidClad2, Materials().FPethylene, nameClad2, 0, 0, 0);
		logicClad2->SetVisAttributes(green);
		logicClad1 = new G4LogicalVolume(solidClad1, Materials().Pethylene, nameClad1, 0, 0, 0);
		logicClad1->SetVisAttributes(green); 
		logicFiber = new G4LogicalVolume(solidFiber, Materials().PMMA, nameFiber, 0, 0, 0);
		logicFiber->SetVisAttributes(green); 
		logicSiPM = new G4LogicalVolume(solidSiPM, Materials().Pyrex, nameSiPM, 0, 0, 0);
		logicSiPM->SetVisAttributes(blue); 

		// physical volumes
		physCoating  = new G4PVPlacement(nullptr, G4ThreeVector(fPosTopX, yp, fPosTopZ), logicCoating, 
			nameCoating, logicCasing, false, barId, fCheckOverlaps);
		physScinBar   = new G4PVPlacement(nullptr, G4ThreeVector(), logicScinBar, 
			nameScinBar, logicCoating, false, barId, fCheckOverlaps);
		physClad2 = new G4PVPlacement(rotationFiber, G4ThreeVector(0, 0, fFiberTopPosZ), logicClad2, 
			nameClad2, logicScinBar, true, barId, fCheckOverlaps);
		physClad1 = new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1, 
			nameClad1, logicClad2, false, barId, fCheckOverlaps);
		physFiber = new G4PVPlacement(nullptr, G4ThreeVector(), logicFiber, 
			nameFiber, logicClad1, false, barId, fCheckOverlaps); 
		physSiPM  = new G4PVPlacement(rotationFiber, G4ThreeVector(fFiberTopPosX, 0, fFiberTopPosZ), logicSiPM, 
			nameSiPM, logicScinBar, false, barId, fCheckOverlaps);

		// registration of SiPM as a Sensitive Volume
		G4MOptDeviceAction* const SiPMTopSD = new G4MOptDeviceAction(newname + nameSiPM, detectorId, barId, theEvent);
		sdMan->AddNewDetector(SiPMTopSD);
		logicSiPM->SetSensitiveDetector(SiPMTopSD);

		
  }

  
	// bars of the bottom panel
	for (G4int i=nBars; i<2*nBars; ++i) {

		// definitions for bar placement
		unsigned int barId = i;
		G4int inc = pow(-1,i);
		G4double xp = inc*fPosBotX;
		
		// naming
		G4String nameCoating = "BarCoating_"+to_string(barId);
		G4String nameScinBar = "BarScin_"+to_string(barId);
		G4String nameClad2 = "FiberClad2_"+to_string(barId);
		G4String nameClad1 = "FiberClad1_"+to_string(barId);
		G4String nameFiber = "Fiber_"+to_string(barId);
		G4String nameSiPM = "SiPM_"+to_string(barId);
		// register SiPM in the detector class
		detector.MakeOptDevice(barId, OptDevice::eSiPM);
		
		// logical volumes
		logicCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, nameCoating, 0, 0, 0);
		logicScinBar  = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, nameScinBar, 0, 0, 0);
		logicClad2 = new G4LogicalVolume(solidClad2, Materials().FPethylene, nameClad2, 0, 0, 0);
		logicClad2->SetVisAttributes(green);
		logicClad1 = new G4LogicalVolume(solidClad1, Materials().Pethylene, nameClad1, 0, 0, 0);
		logicClad1->SetVisAttributes(green); 
		logicFiber = new G4LogicalVolume(solidFiber, Materials().PMMA, nameFiber, 0, 0, 0);
		logicFiber->SetVisAttributes(green); 
		logicSiPM = new G4LogicalVolume(solidSiPM, Materials().Pyrex, nameSiPM, 0, 0, 0);
		logicSiPM->SetVisAttributes(blue);
		
		// physical volumes
		physCoating  = new G4PVPlacement(rotationBot, G4ThreeVector(xp, fPosBotY, fPosBotZ), logicCoating, 
			nameCoating, logicCasing, false, barId, fCheckOverlaps);
		physScinBar   = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logicScinBar,
			nameScinBar, logicCoating, false, barId, fCheckOverlaps);
		physClad2 = new G4PVPlacement(rotationFiber, G4ThreeVector(0, 0, fFiberBotPosZ), logicClad2, 
			nameClad2, logicScinBar, true, barId, fCheckOverlaps);
		physClad1 = new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1, 
			nameClad1, logicClad2, false, barId, fCheckOverlaps);
		physFiber = new G4PVPlacement(nullptr, G4ThreeVector(), logicFiber, 
			nameFiber, logicClad1, false, barId, fCheckOverlaps); 
		physSiPM  = new G4PVPlacement(rotationFiber, G4ThreeVector(fFiberTopPosX, 0, fFiberBotPosZ), logicSiPM, 
		nameSiPM, logicScinBar, false, barId, fCheckOverlaps);

		// registration of SiPM as a Sensitive Volume
		G4MOptDeviceAction* const SiPMBotSD = new G4MOptDeviceAction(newname + nameSiPM, detectorId, barId, theEvent);
		sdMan->AddNewDetector(SiPMBotSD);
		logicSiPM->SetSensitiveDetector(SiPMBotSD);
	
  }

}
