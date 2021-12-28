#include "Mudulus.h"
#include "Geometry.h"
#include "G4MPixelAction.h"
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
Mudulus::BuildDetector(G4LogicalVolume* logMother, Module& module, Event& theEvent, G4int fBarsPanel, G4bool fCheckOverlaps)
{

	G4SDManager* const sdMan = G4SDManager::GetSDMpointer();

	// module position
	G4ThreeVector  modulePos = Geometry::ToG4Vector(module.GetModulePosition(), 1.);
	int moduleId = module.GetId();
	int nBars = module.GetNBars();
	// module properties
	
	std::cout << "[DEBUG] Mudulus::BuildDetector: Creating Detector Mudulus with " << nBars << " bars." << std::endl;

	// scintillator bar properties
	fBarWidth  = module.GetBarWidth() * mm;
	fBarLength = module.GetBarLength(); 
	fBarThickness = module.GetBarThickness() * mm;
	fCoatingThickness = module.GetBarCoatingThickness() * mm;
	G4double fHalfWidth = 0.5*fBarWidth*nBars;

	// fiber properties
	fCladdingThickness = module.GetCladdingThickness() * mm;
	fFiberRadius = module.GetFiberRadius() * mm;

	SiPM& sipm = module.GetSiPM();
	fSiPMSizeZ = sipm.GetSiPMWidth() * mm;
	fSiPMSizeX = fSiPMSizeY = sipm.GetSiPMLength() * mm;

	// define a enclosure volume that contains the components of the module
	// this is just to ease the module construction...not a proper volume
	ostringstream nameModule;
	nameModule.str("");
	nameModule << "Casing" << '_' << moduleId;
	fCasingSizeX = fBarLength;
	fCasingSizeY = fBarLength;
	fCasingSizeZ = 0.5*fBarThickness * 2; // (x2) number of panels 
	fCasingThickness = module.GetCasingThickness() * mm;
	
	// Casing
	solidCasing = new G4Box("Casing", fCasingSizeX+fCasingThickness,fCasingSizeY+fCasingThickness, fCasingSizeZ+fCasingThickness);
	logicCasing = new G4LogicalVolume(solidCasing, Materials().Air, "Casing", 0, 0, 0);
	physCasing  = new G4PVPlacement(nullptr, modulePos, logicCasing, "Casing", logMother, false, moduleId, fCheckOverlaps);
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
#warning "Change to PMT!"
	// SiPM
	solidSiPM   = new G4Box("SiPM", fSiPMSizeX, fSiPMSizeY, fSiPMSizeZ);
	//std::cout << "[DEBUG] G4Models::Musaic: Building SiPM! " << std::endl;
	
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
	G4double fPixelPosZ = 0.5*fBarLength + 0.5*fFiberExtra - 0.5*fSiPMSizeZ;

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
		string nameSiPMl = "Pixel_left_"+panelId+"_"+to_string(barId);
		string nameSiPMr = "Pixel_right_"+panelId+"_"+to_string(barId);

		// register SiPM in the detector class
		module.MakeSiPM(barId);

		logicCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, nameCoating, 0, 0, 0);
		logicScinBar  = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, nameScinBar, 0, 0, 0);
		logicClad2 = new G4LogicalVolume(solidClad2, Materials().FPethylene, nameClad2, 0, 0, 0);
		logicClad2->SetVisAttributes(green);
		logicClad1 = new G4LogicalVolume(solidClad1, Materials().Pethylene, nameClad1, 0, 0, 0);
		logicClad1->SetVisAttributes(green); 
		logicFiber = new G4LogicalVolume(solidFiber, Materials().PMMA, nameFiber, 0, 0, 0);
		logicFiber->SetVisAttributes(green); 
		logicSiPMl = new G4LogicalVolume(solidSiPM, Materials().Pyrex, nameSiPMl, 0, 0, 0);
		logicSiPMl->SetVisAttributes(blue); 
		logicSiPMr = new G4LogicalVolume(solidSiPM, Materials().Pyrex, nameSiPMr, 0, 0, 0);
		logicSiPMr->SetVisAttributes(blue);

		// top panel positon
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
		physSiPMl  = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fPixelPosZ), logicSiPMl,
			nameSiPMl, logicFiber, false, barId, fCheckOverlaps);
		physSiPMr  = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, -1*fPixelPosZ), logicSiPMr,
			nameSiPMr, logicFiber, false, barId, fCheckOverlaps);

		// registration of SiPM
		G4MPixelAction* const SiPMTopSD = new G4MPixelAction("/Mudulus/" + nameModule.str() + "/" + nameSiPMl, moduleId, barId, theEvent);
		sdMan->AddNewDetector(SiPMTopSD);
		logicSiPMl->SetSensitiveDetector(SiPMTopSD);

		
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
		string nameSiPMl = "Pixel_left_"+panelId+"_"+to_string(barId);
		string nameSiPMr = "Pixel_right_"+panelId+"_"+to_string(barId);

		// register SiPM in the detector class
		module.MakeSiPM(barId);

		logicCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, nameCoating, 0, 0, 0);
		logicScinBar  = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, nameScinBar, 0, 0, 0);
		logicClad2 = new G4LogicalVolume(solidClad2, Materials().FPethylene, nameClad2, 0, 0, 0);
		logicClad2->SetVisAttributes(green);
		logicClad1 = new G4LogicalVolume(solidClad1, Materials().Pethylene, nameClad1, 0, 0, 0);
		logicClad1->SetVisAttributes(green); 
		logicFiber = new G4LogicalVolume(solidFiber, Materials().PMMA, nameFiber, 0, 0, 0);
		logicFiber->SetVisAttributes(green); 
		logicSiPMl = new G4LogicalVolume(solidSiPM, Materials().Pyrex, nameSiPMl, 0, 0, 0);
		logicSiPMl->SetVisAttributes(blue);
		logicSiPMr = new G4LogicalVolume(solidSiPM, Materials().Pyrex, nameSiPMr, 0, 0, 0);
		logicSiPMr->SetVisAttributes(blue);

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
		physSiPMl  = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fPixelPosZ), logicSiPMl, 
				nameSiPMl, logicFiber, false, barId, fCheckOverlaps);
		physSiPMr  = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, -1*fPixelPosZ), logicSiPMr, 
				nameSiPMr, logicFiber, false, barId, fCheckOverlaps);

		// registration of SiPM
		G4MPixelAction* const SiPMBotSD = new G4MPixelAction("/Mudulus/" + nameModule.str() + "/" + nameSiPMl, moduleId, barId, theEvent);
		sdMan->AddNewDetector(SiPMBotSD);
		logicSiPMl->SetSensitiveDetector(SiPMBotSD);

	}

	
}
