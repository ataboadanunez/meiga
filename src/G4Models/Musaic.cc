#include "Musaic.h"
#include "Geometry.h"
#include "G4MSiPMAction.h"
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
Musaic::BuildDetector(G4LogicalVolume* logMother, Module& module, Event& theEvent, G4int fBarsPanel, G4bool fCheckOverlaps)
{

	std::cout << "[DEBUG] G4Models::Musaic: Building Detector Musaic! " << std::endl;
	G4SDManager* const sdMan = G4SDManager::GetSDMpointer();

	// module position
	G4ThreeVector  modulePos = G4ThreeVector(0., 0., 0.);
	//Geometry::ToG4Vector(module.GetModulePosition(), 1.);
	auto pos = module.GetModulePosition();
	//std::cout << "[DEBUG] G4Models::Musaic: Module position = " << pos[0] << std::endl;
	int moduleId = module.GetId();
	int nBars = module.GetNBars();
	std::cout << "[DEBUG] G4Models::Musaic: Number of Bars per panel = " << nBars << std::endl;
	// module properties
	fCasingThickness = module.GetCasingThickness() * mm;

	// scintillator bar properties
	fBarWidth  = module.GetBarWidth() * mm;
	fBarLength = module.GetNBars() * fBarWidth; 
	fBarThickness = module.GetBarThickness() * mm;
	fCoatingThickness = module.GetBarCoatingThickness() * mm;

	// fiber properties
	fCladdingThickness = module.GetCladdingThickness() * mm;
	fFiberRadius = module.GetFiberRadius() * mm;

	fCasingSizeX = fBarLength/2 + fCoatingThickness + fCasingThickness;
	fCasingSizeY = fBarWidth/2 + fCoatingThickness;// + fCasingThickness;
	fCasingSizeZ = fBarThickness/2 + fCoatingThickness;// + fCasingThickness;

	SiPM& sipm = module.GetSiPM();
	fSiPMSizeZ = sipm.GetSiPMWidth() * mm;
	fSiPMSizeX = fSiPMSizeY = sipm.GetSiPMLength() * mm;

	fCasingSizeX = fBarLength/2 + fCoatingThickness + fCasingThickness;
	fCasingSizeY = fBarWidth/2 + fCoatingThickness;
	fCasingSizeZ = fBarThickness/2 + fCoatingThickness;

	// define a enclosure volume that contains the components of the module
	// this is just to ease the module construction...not a proper volume
	ostringstream nameModule;
	nameModule.str("");
	nameModule << "Casing" << '_' << moduleId;
	const double x = fBarLength/2;
	const double y = fBarWidth/2 * nBars; // number of bars on each panel
	const double z = fBarThickness/2 * 2; // 2 = number of panels
	const double t = fCasingThickness;
	std::cout << "[DEBUG] G4Models::Musaic: up to here all good! " << std::endl;
	// Casing
	solidCasing = new G4Box("Casing", x+t, y+t, z+t);
	logicCasing = new G4LogicalVolume(solidCasing, Materials().Air, "Casing", 0, 0, 0);
	physCasing  = new G4PVPlacement(nullptr, modulePos, logicCasing, "Casing", logMother, false, moduleId, fCheckOverlaps);
	std::cout << "[DEBUG] G4Models::Musaic: Building Casing! " << std::endl;
	// Bars: Coating + Scintillator bar
	solidCoating  	= new G4Box("BarCoating", fCoatingThickness + fBarLength/2, fCoatingThickness + fBarWidth/2, fCoatingThickness + fBarThickness/2);
	solidScinBar   	= new G4Box("BarScin", fBarLength/2, fBarWidth/2, fBarThickness/2);
	std::cout << "[DEBUG] G4Models::Musaic: Building Bars! " << std::endl;
	// Fiber: Cladding (external & internal) + fiber core
	solidClad2  = new G4Tubs("Clad2", 0, fFiberRadius, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	solidClad1  = new G4Tubs("Clad1", 0, fFiberRadius - fCladdingThickness, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	solidFiber  = new G4Tubs("Fiber", 0, fFiberRadius - 2*fCladdingThickness, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	std::cout << "[DEBUG] G4Models::Musaic: Building Fibers! " << std::endl;
	// SiPM
	solidSiPM   = new G4Box("SiPM", fSiPMSizeX, fSiPMSizeY, fSiPMSizeZ);
	std::cout << "[DEBUG] G4Models::Musaic: Building SiPM! " << std::endl;
	
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

	/****
		coordinates and orientation of module components
		bar length 		along x-axis
		bar width 		along y-axis
		bar thickness along z-axis

		in the (MxN) = (2x2) configuration, 2 bars will be placed
		along the y axis (M) and 2 along the z axis (N). therefore
		bars should be created within the loop and placed accordingly

		position of top and bottom bars are set wrt module origin

	*/

	G4double fPosTopX = 0;
	G4double fPosTopY = fCasingSizeY;
	G4double fPosTopZ = fCasingSizeZ;

	G4double fPosBotX = fCasingSizeY;
	G4double fPosBotY = 0;
	G4double fPosBotZ = -fCasingSizeZ;
	// place fiber at the top of the bar (x-axis)
	// place SiPM at the end of the bar (z-axis)
	G4double fFiberTopPosX = fBarLength/2 - 2*fCoatingThickness;
	G4double fFiberTopPosZ = -fBarThickness/2 + fCoatingThickness + fFiberRadius + fCladdingThickness;
	G4double fFiberBotPosZ = fFiberTopPosZ;

	// bars of the top panel
	for (G4int i=0; i<nBars; ++i) {

		/* 
			update bar placement for nBars>2...
			maybe create consecutive bars starting form a given point, i.e.,
			barPos = MoulePosition - nBars * barLength (?) or smth like this...
		*/

		unsigned int barId = i;
		G4int inc = pow(-1,i);//(1+2*i);
		G4double yp = inc*fPosTopY;

		string nameCoating = "BarCoating_"+to_string(barId);
		string nameScinBar = "BarScin_"+to_string(barId);
		string nameClad2 = "FiberClad2_"+to_string(barId);
		string nameClad1 = "FiberClad1_"+to_string(barId);
		string nameFiber = "Fiber_"+to_string(barId);
		string nameSiPM = "SiPM_"+to_string(barId);

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
		logicSiPM = new G4LogicalVolume(solidSiPM, Materials().Pyrex, nameSiPM, 0, 0, 0);
		logicSiPM->SetVisAttributes(blue); 

		// top panel upper direction
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

		// registration of SiPM
		G4MSiPMAction* const SiPMTopSD = new G4MSiPMAction("/Module/" + nameModule.str() + "/" + nameSiPM, moduleId, barId, theEvent);
		sdMan->AddNewDetector(SiPMTopSD);
		logicSiPM->SetSensitiveDetector(SiPMTopSD);

		
  }

  
	// bars of the bottom panel
	for (G4int i=nBars; i<2*nBars; ++i) {

		unsigned int barId = i;
		G4int inc = pow(-1,i);
		G4double xp = inc*fPosBotX;

		string nameCoating = "BarCoating_"+to_string(barId);
		string nameScinBar = "BarScin_"+to_string(barId);
		string nameClad2 = "FiberClad2_"+to_string(barId);
		string nameClad1 = "FiberClad1_"+to_string(barId);
		string nameFiber = "Fiber_"+to_string(barId);
		string nameSiPM = "SiPM_"+to_string(barId);

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
		logicSiPM = new G4LogicalVolume(solidSiPM, Materials().Pyrex, nameSiPM, 0, 0, 0);
		logicSiPM->SetVisAttributes(blue);

		// upper direction
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

		// registration of SiPM
		G4MSiPMAction* const SiPMBotSD = new G4MSiPMAction("/Module/" + nameModule.str() + "/" + nameSiPM, moduleId, barId, theEvent);
		sdMan->AddNewDetector(SiPMBotSD);
		logicSiPM->SetSensitiveDetector(SiPMBotSD);
	
  }

}
