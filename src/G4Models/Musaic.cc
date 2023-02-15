#include "Musaic.h"
#include "Geometry.h"
#include "G4MOptDeviceAction.h"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include <iostream>

using namespace std;

void 
Musaic::BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOverlaps)
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

	G4Box* solidSiPM = nullptr;

	// logical volumes
	G4LogicalVolume* logicCasing = nullptr;
	G4LogicalVolume* logicCoating = nullptr;
	G4LogicalVolume* logicScinBar = nullptr;

	G4LogicalVolume* logicClad2 = nullptr;
	G4LogicalVolume* logicClad1 = nullptr;
	G4LogicalVolume* logicFiber = nullptr;

	G4LogicalVolume* logicSiPM = nullptr;
	
	// detector properties 
	const G4double fCasingThickness = detector.GetCasingThickness() * CLHEP::mm;
	
	// scintillator bar properties
#warning "Bar size parameters are fixed for this detector"
	const G4double fBarWidth  = 41*CLHEP::mm;
	const G4double fBarLength = 82*CLHEP::mm;
	const G4double fBarThickness = 10*CLHEP::mm;
	const G4double fCoatingThickness = detector.GetBarCoatingThickness();
	const G4int  nBars = 2;
	
	// fiber properties
	const G4double fCladdingThickness = detector.GetCladdingThickness();
	const G4double fFiberRadius = detector.GetFiberRadius();

	// Optical device properties
	OptDevice sipm = detector.GetOptDevice(OptDevice::eSiPM);
	const G4double fSiPMSizeX = sipm.GetLength() * CLHEP::mm;
	const G4double fSiPMSizeY = sipm.GetWidth() * CLHEP::mm;
	const G4double fSiPMSizeZ = sipm.GetThickness() * CLHEP::mm;
	
	// define a enclosure volume that contains the components of the detector
	const G4double fCasingSizeX = 0.5*fBarLength + fCasingThickness;
	const G4double fCasingSizeY = 0.5*fBarLength + fCasingThickness;
	const G4double fCasingSizeZ = 0.5*fBarThickness * 2 + fCasingThickness; // (x2 = number of panels)

	G4SDManager* const sdMan = G4SDManager::GetSDMpointer();
	auto pos = detector.GetDetectorPosition();
	auto  detectorPos = Geometry::ToG4Vector(pos, 1.);
	const G4int  detectorId = detector.GetId();

	ostringstream namedetector;
	namedetector.str("");
	namedetector << "Musaic" << '_' << detectorId;
	cout << "G4Models::Musaic: Building detector " << namedetector.str();
	cout << " with " << sipm.GetName() << ". " << endl;
	
	/****************************
		
		Geant4 Volume construction
	
	****************************/ 

	// Casing
	solidCasing = new G4Box("Casing", fCasingSizeX, fCasingSizeY, fCasingSizeZ);
	logicCasing = new G4LogicalVolume(solidCasing, Materials().Air, "Casing", 0, 0, 0);
	new G4PVPlacement(nullptr, detectorPos, logicCasing, "Casing", logMother, false, detectorId, fCheckOverlaps);
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
		int barId = i+1;
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
		new G4PVPlacement(nullptr, G4ThreeVector(fPosTopX, yp, fPosTopZ), logicCoating, 
			nameCoating, logicCasing, false, barId, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(), logicScinBar, 
			nameScinBar, logicCoating, false, barId, fCheckOverlaps);
		new G4PVPlacement(rotationFiber, G4ThreeVector(0, 0, fFiberTopPosZ), logicClad2, 
			nameClad2, logicScinBar, true, barId, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1, 
			nameClad1, logicClad2, false, barId, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(), logicFiber, 
			nameFiber, logicClad1, false, barId, fCheckOverlaps); 
		new G4PVPlacement(rotationFiber, G4ThreeVector(fFiberTopPosX, 0, fFiberTopPosZ), logicSiPM, 
			nameSiPM, logicScinBar, false, barId, fCheckOverlaps);

		// registration of SiPM as a Sensitive Volume
		G4MOptDeviceAction* const SiPMTopSD = new G4MOptDeviceAction(namedetector.str() + "/" + nameSiPM, detectorId, barId, theEvent);
		sdMan->AddNewDetector(SiPMTopSD);
		logicSiPM->SetSensitiveDetector(SiPMTopSD);

		
	}

	
	// bars of the bottom panel
	for (G4int i=0; i<nBars; ++i) {

		// definitions for bar placement
		unsigned int barId = i+1+nBars;
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
		new G4PVPlacement(rotationBot, G4ThreeVector(xp, fPosBotY, fPosBotZ), logicCoating, 
			nameCoating, logicCasing, false, barId, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logicScinBar,
			nameScinBar, logicCoating, false, barId, fCheckOverlaps);
		new G4PVPlacement(rotationFiber, G4ThreeVector(0, 0, fFiberBotPosZ), logicClad2, 
			nameClad2, logicScinBar, true, barId, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1, 
			nameClad1, logicClad2, false, barId, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(), logicFiber, 
			nameFiber, logicClad1, false, barId, fCheckOverlaps); 
		new G4PVPlacement(rotationFiber, G4ThreeVector(fFiberTopPosX, 0, fFiberBotPosZ), logicSiPM, 
		nameSiPM, logicScinBar, false, barId, fCheckOverlaps);

		// registration of SiPM as a Sensitive Volume
		G4MOptDeviceAction* const SiPMBotSD = new G4MOptDeviceAction(namedetector.str() + "/" + nameSiPM, detectorId, barId, theEvent);
		sdMan->AddNewDetector(SiPMBotSD);
		logicSiPM->SetSensitiveDetector(SiPMBotSD);
	
	}

}
