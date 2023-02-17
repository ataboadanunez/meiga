// implementation of the class Scintillator
// Meiga headers
#include "Scintillator.h"
#include "Geometry.h"
#include "G4MDetectorAction.h"

// Geant4 headers
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
// C++
#include <iostream>

using namespace std;

Scintillator::Scintillator()
{

}

Scintillator::~Scintillator()
{

}

void 
Scintillator::BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOverlaps)
{


	int detectorId = detector.GetId();
	G4ThreeVector  detectorPos = Geometry::ToG4Vector(detector.GetDetectorPosition(), 1.);

	ostringstream namedetector;
	namedetector.str("");
	namedetector << "Scintillator";
	cout << "[INFO] G4Models::Scintillator: Building detector " << namedetector.str();
	cout << " (ID = " << detectorId << ")" << endl;
	cout << "[INFO] G4Models::Scintillator: Detector Position = ";
	cout << "(" << detectorPos.getX() / CLHEP::cm << ", " << detectorPos.getY() << ", " << detectorPos.getZ() << ") " << endl;

	// solids
	G4Box* solidCoating = nullptr;
	G4Box* solidScinBar = nullptr;

	// logical volumes
	G4LogicalVolume* logicCoating = nullptr;
	G4LogicalVolume* logicScinBar = nullptr;

	// following variables are read from Detector class
	G4double fCoatingThickness = detector.GetBarCoatingThickness();
	G4double fBarWidth = detector.GetBarWidth();
	G4double fBarLength = detector.GetBarLength();
	G4double fBarThickness = detector.GetBarThickness();
	G4int fNBars = detector.GetNBars();
	G4double fHalfWidth = 0.5*fBarWidth*fNBars; 

	G4SDManager* const sdMan = G4SDManager::GetSDMpointer();
	
	// Bars: Coating + Scintillator bar
	solidCoating  	= new G4Box("BarCoating", 0.5*fBarLength + fCoatingThickness, 0.5*fBarWidth + fCoatingThickness, 0.5*fBarThickness + fCoatingThickness);
	solidScinBar   	= new G4Box("BarScin", 0.5*fBarLength, 0.5*fBarWidth, 0.5*fBarThickness);
	// register Bar coating as a LogicalSkinSurface
	new G4LogicalSkinSurface("BarCoating", logicCoating, Materials().ScinOptSurf);

	// logical volumes
	logicCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, "BarCoating", 0, 0, 0);
	logicScinBar = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, "BarScin", 0, 0, 0);

	// bars placement
	for (G4int i=0; i<fNBars; ++i) {

		// definitions for bar placement
		int barId = i+1;

		// naming
		G4String nameCoating = "BarCoating_"+to_string(barId);
		G4String nameScinBar = "BarScin_"+to_string(barId);
		
		// place bars along the y-axis
		G4double yPos = detectorPos.getY() + i*(fBarWidth + 2*fCoatingThickness);
		// shift each bar position by 1/2 of the panel width to center the detector
		yPos -= fHalfWidth;

		

		// physical volumes
		new G4PVPlacement(nullptr, G4ThreeVector(detectorPos.getX(), yPos, detectorPos.getZ()), logicCoating, 
			nameCoating, logMother, false, barId, fCheckOverlaps);
		new G4PVPlacement(nullptr, G4ThreeVector(), logicScinBar, 
			nameScinBar, logicCoating, false, barId, fCheckOverlaps);

  }

  // register scintillator bar as sensitive detector
	G4MDetectorAction* const scinSD = new G4MDetectorAction("BarScin", detectorId, theEvent);
	sdMan->AddNewDetector(scinSD);
	logicScinBar->SetSensitiveDetector(scinSD);

  

}
