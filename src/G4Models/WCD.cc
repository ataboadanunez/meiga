#include <iostream>

#include "WCD.h"
#include "Geometry.h"
#include "G4MPMTAction.h"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

using namespace std;

void WCD::BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOVerlaps)
{

	// optical and border sufraces
	G4OpticalSurface* LinerOptSurf = nullptr;
	G4LogicalBorderSurface* topSurface = nullptr;
	G4LogicalBorderSurface* botSurface = nullptr;
	G4LogicalBorderSurface* sideSurface = nullptr;

	// solids
	G4Tubs* solidTank = nullptr;
	G4Tubs* solidTop  = nullptr;
	G4Tubs* solidBot  = nullptr;
	G4Tubs* solidSide = nullptr;

	// pmt solids
	G4Ellipsoid* solidPMT = nullptr;


	// logical volumes
	G4LogicalVolume* logTank = nullptr;
	G4LogicalVolume* logTop  = nullptr;
	G4LogicalVolume* logBot  = nullptr;
	G4LogicalVolume* logSide = nullptr;

	G4LogicalVolume* logPMT = nullptr;

	// physical volumes
	G4PVPlacement* physTank = nullptr;
	G4PVPlacement* physTop  = nullptr;
	G4PVPlacement* physBot  = nullptr;
	G4PVPlacement* physSide = nullptr;

	G4PVPlacement* physPMT = nullptr;
	// Auger's WCD dimensions
	G4double fTankRadius = 1.8 * CLHEP::m;
	G4double fTankHeight = 1.2 * CLHEP::m;
	G4double fTankHalfHeight = 0.5 * fTankHeight;
	G4double fTankThickness = 12.7 * CLHEP::mm;
	// Auger's photonis-XP1805
	G4double fPmtRmin = 114 * CLHEP::mm;
	G4double fPmtRzmin = 84.5781 * CLHEP::mm;

	G4ThreeVector detectorPos = Geometry::ToG4Vector(detector.GetDetectorPosition(), 1.);

	G4double fTankPosX = detectorPos.getX();
	G4double fTankPosY = detectorPos.getY();
	G4double fTankPosZ = detectorPos.getZ();

	// pmt position is set relative to tank center
	G4ThreeVector fTankCenter = detectorPos + G4ThreeVector(0, 0, fTankHalfHeight + fTankThickness);


	solidTank = new G4Tubs("Tank", 0, fTankRadius, fTankHalfHeight, 0, 360*deg);
	solidTop = new G4Tubs("Top", 0, fTankRadius + fTankThickness, fTankThickness/2, 0, 360*deg);
	solidSide = new G4Tubs("Side", fTankRadius, fTankRadius + fTankThickness, fTankHalfHeight, 0, 360*deg);

	// pmt solids (G4StationConstruction.cc:1416)
	solidPMT = new G4Ellipsoid("PMT", fPmtRmin, fPmtRmin, fPmtRzmin, -fPmtRzmin, 0);

	// assemble WCD 
	G4SDManager* const sdMan = G4SDManager::GetSDMpointer();
	// water part
	logTank  = new G4LogicalVolume(solidTank, Materials().Water, "logTank", 0, 0, 0);
	physTank = new G4PVPlacement(nullptr, fTankCenter, logTank, "physTank", logMother, false, 0, fCheckOVerlaps);
	
	// top, bottom and side walls of the tank
	logTop  = new G4LogicalVolume(solidTop, Materials().HDPE, "logTop", 0, 0, 0);
	physTop = new G4PVPlacement(nullptr, G4ThreeVector(fTankPosX, fTankPosY, fTankPosZ + 2*fTankHalfHeight + 1.5*fTankThickness), logTop, "physTop", logMother, false, 0, fCheckOVerlaps);
	logBot  = new G4LogicalVolume(solidBot, Materials().HDPE, "logBot", 0, 0, 0);
	physBot = new G4PVPlacement(nullptr, G4ThreeVector(fTankPosX, fTankPosY, fTankPosZ + 0.5*fTankThickness), logTop, "physBot", logMother, false, 0, fCheckOVerlaps);
	logSide  = new G4LogicalVolume(solidSide, Materials().HDPE, "logSide", 0, 0, 0);
	physSide = new G4PVPlacement(nullptr, G4ThreeVector(fTankPosX, fTankPosY, fTankPosZ + fTankHalfHeight + fTankThickness), logSide, "physSide", logMother, false, 0, fCheckOVerlaps);

	

	// tank liner surface
	new G4LogicalBorderSurface("TopSurface", physTank, physTop, Materials().LinerOptSurf);
	new G4LogicalBorderSurface("BotSurface", physTank, physBot, Materials().LinerOptSurf);
	new G4LogicalBorderSurface("SideSurface", physTank, physSide, Materials().LinerOptSurf);

	// PMT
	logPMT = new G4LogicalVolume(solidPMT, Materials().Pyrex, "logPMT", 0, 0, 0);
	physPMT = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fTankHalfHeight), logPMT, "physPMT", logTank, false, 0, fCheckOVerlaps);

	// register the PMT as sensitive detectors
	ostringstream name;
	name.str("");
	name << "/WCD/pmt";
# warning "include PMT ids?"
	G4MPMTAction* const pmtSD = new G4MPMTAction(name.str().c_str(), 0, 0, theEvent);
	sdMan->AddNewDetector(pmtSD);
	logPMT->SetSensitiveDetector(pmtSD);

}