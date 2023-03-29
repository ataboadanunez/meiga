#include "Hodoscope.h"
#include "Geometry.h"
#include "Materials.h"

#include "G4SDManager.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"

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

	// logical volumes
	G4LogicalVolume* logEnclosure = nullptr;
	G4LogicalVolume* logCasing = nullptr;
	G4LogicalVolume* logCoating = nullptr;
	G4LogicalVolume* logScinBar = nullptr;

	// Configuration of scintillator panels
	// number of panels of scintillator bars
	// each panel consists of grid of 2*N_bars
	int nPanels = detector.GetNPanels();
	// number of scintillator bars
	int nBars = detector.GetNBars();
	// distance between panels
	double distanceBtwPanels = detector.GetDistanceBtwPanels();

	// Scintillator bars
	double barWidth = detector.GetBarWidth();
	double barLength = detector.GetBarLength();
	double barThickness = detector.GetBarThickness();
	double barCoatingThickness = detector.GetBarCoatingThickness();
	double detectorWidth = nBars * (barWidth + barCoatingThickness);
	double halfWidth = 0.5 * nBars * (barWidth + barCoatingThickness);
	// detector
	G4ThreeVector detectorPos = Geometry::ToG4Vector(detector.GetDetectorPosition(), 1.);
	int detectorId = detector.GetId();
	ostringstream namedetector;
	namedetector.str("");
	namedetector << "Hodoscope";
	cout << "[INFO] Hodoscope::BuildDetector: Building detector " << namedetector.str();
	cout << " (ID = " << detectorId << ")";
	cout << " with " << nBars << " scintillator bars, " << nPanels << " panels.";
	cout << " Distance between panels = " << distanceBtwPanels / CLHEP::cm << " cm. " << endl;

	// determine casing size
	double maxLength;
	maxLength = (barLength > detectorWidth ? barLength : detectorWidth);

	const double fCasingThickness = 2 * CLHEP::mm;
	const double fCasingSizeX = maxLength + fCasingThickness;
	const double fCasingSizeY = maxLength + fCasingThickness;
	const double fCasingSizeZ = barThickness * 4;

	const double fEnclosureSizeX = fCasingSizeX;
	const double fEnclosureSizeY = fCasingSizeY;
	const double fEnclosureSizeZ = fCasingSizeZ + 2 * distanceBtwPanels;

	solidEnclosure = new G4Box("Enclosure", 0.5*fEnclosureSizeX, 0.5*fEnclosureSizeY, 0.5*fEnclosureSizeZ);
	solidCasing = new G4Box("Casing", 0.5*fCasingSizeX, 0.5*fCasingSizeY, 0.5*fCasingSizeZ);
	solidCoating = new G4Box("BarCoating", 0.5*barLength + barCoatingThickness,
																				 0.5*barWidth + barCoatingThickness,
																				 0.5*barThickness + barCoatingThickness);
	solidScinBar = new G4Box("BarScin", 0.5*barLength,
																			0.5*barWidth,
																			0.5*barThickness);


	// useful rotation matrix for bars orientation
	G4RotationMatrix* rotationEnclosure = new G4RotationMatrix();
	G4RotationMatrix* rotationBot = new G4RotationMatrix();
	double alpha = detector.GetRotationAngle();
	cout << "[INFO] Hodoscope::BuildDetector: Rotation Angle = " << alpha << endl;
	rotationEnclosure->rotateX(alpha);
	rotationBot->rotateZ(M_PI/2);

	logEnclosure = new G4LogicalVolume(solidEnclosure, Materials().Air, "Enclosure", 0, 0, 0);
	new G4PVPlacement(rotationEnclosure, G4ThreeVector(detectorPos.getX(), detectorPos.getY(), detectorPos.getZ()), logEnclosure, "Enclosure", logMother, false, detectorId, fCheckOverlaps);

	// loop over panels
	for (int pIt=0; pIt<nPanels; ++pIt) {

		// panel position w.r.t enclosure 
		double panelPosX = 0.;
		double panelPosY = 0.;
		double panelPosZ = 0.;

		// shift top and bottom panels z-coordinate
		if (pIt==0)
			panelPosZ += distanceBtwPanels;
		else if (pIt==1)
			panelPosZ += 0;
		else if (pIt==2)
			panelPosZ += -1*distanceBtwPanels;

		G4ThreeVector panelPosition = G4ThreeVector(panelPosX, panelPosY, panelPosZ);

		int panelId = pIt + 1;
		string nameCasing = "Casing_"+to_string(panelId);

		logCasing = new G4LogicalVolume(solidCasing, Materials().Air, nameCasing, 0, 0, 0);
		new G4PVPlacement(nullptr, panelPosition, logCasing, nameCasing, logEnclosure, false, panelId, fCheckOverlaps);

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

			// logical volumes
			logCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, nameCoating, 0, 0, 0);
			logScinBar = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, nameScinBar, 0, 0, 0);

			// physical volumes
			new G4PVPlacement(nullptr, barPosition, logCoating, nameCoating, logCasing, false, barId2, fCheckOverlaps);
			new G4PVPlacement(nullptr, G4ThreeVector(), logScinBar, nameScinBar, logCoating, false, barId2, fCheckOverlaps);

		} // end loop of batrs of the top panel

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

			// logical volumes
			logCoating = new G4LogicalVolume(solidCoating, Materials().ScinCoating, nameCoating, 0, 0, 0);
			logScinBar = new G4LogicalVolume(solidScinBar, Materials().ScinPlastic, nameScinBar, 0, 0, 0);

			// physical volumes
			new G4PVPlacement(rotationBot, barPosition, logCoating, nameCoating, logCasing, false, barId2, fCheckOverlaps);
			new G4PVPlacement(nullptr, G4ThreeVector(), logScinBar, nameScinBar, logCoating, false, barId2, fCheckOverlaps);

		} // end loop of batrs of the top panel

	}


}