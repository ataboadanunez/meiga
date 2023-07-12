#include <iostream>

#include "SaltyWCD.h"
#include "Geometry.h"
#include "G4MDetectorAction.h"
#include "G4MPMTAction.h"


#include "G4VisAttributes.hh"
#include "G4NistManager.hh"
#include "G4Colour.hh"

using namespace std;

void 
SaltyWCD::BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOVerlaps)
{

	
	// solids
	G4Tubs* solidCasingTop = nullptr;
	G4Tubs* solidCasingSide = nullptr;

	G4Tubs* solidTank = nullptr;
	G4Tubs* solidTop  = nullptr;
	G4Tubs* solidBot  = nullptr;
	G4Tubs* solidSide = nullptr;

	// pmt solids
	G4Ellipsoid* solidPMT = nullptr;

	// logical volumes
	G4LogicalVolume* logCasingTop = nullptr;
	G4LogicalVolume* logCasingBot = nullptr;
	G4LogicalVolume* logCasingSide = nullptr;

	G4LogicalVolume* logTank = nullptr;
	G4LogicalVolume* logTop  = nullptr;
	G4LogicalVolume* logBot = nullptr;
	G4LogicalVolume* logSide = nullptr;

	G4LogicalVolume* logPMT = nullptr;

	// physical volumes
	G4PVPlacement* physCasingBot = nullptr;
	G4PVPlacement* physCasingTop = nullptr;
	G4PVPlacement* physCasingSide = nullptr;

	G4PVPlacement* physTank = nullptr;
	G4PVPlacement* physBot  = nullptr;
	G4PVPlacement* physTop  = nullptr;
	G4PVPlacement* physSide = nullptr;

	// SaltyWCD dimensions
	G4double fTankRadius = detector.GetTankRadius();
	G4double fTankHeight = detector.GetTankHeight();
	G4double fTankHalfHeight = 0.5 * fTankHeight;
	G4double fTankThickness = detector.GetTankThickness();
	G4double fNaClFracMass = detector.GetImpuritiesFraction();
	// --------------------------------------------------------------------
	// SaltyWater defined here for this particular detector
	// --------------------------------------------------------------------
	G4Material* SaltyWater = new G4Material("SaltyWater", 1.1 * g/cm3, 2);
	SaltyWater->AddMaterial(Materials().Water,1-fNaClFracMass);
	SaltyWater->AddMaterial(Materials().Salt, fNaClFracMass);
	SaltyWater->SetMaterialPropertiesTable(Materials().waterPT1);

	G4NistManager* nist = G4NistManager::Instance();
	G4Material* StainlessSteel = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
	
	// PMT properties photonis-XP1805
	OptDevice pmt = detector.GetOptDevice(OptDevice::ePMT);
	G4double fPMTSemiX = pmt.GetSemiAxisX() * CLHEP::cm;
	G4double fPMTSemiY = pmt.GetSemiAxisY() * CLHEP::cm;
	G4double fPMTSemiZ = pmt.GetSemiAxisZ() * CLHEP::cm;

	G4ThreeVector detectorPos = Geometry::ToG4Vector(detector.GetDetectorPosition(), 1.);
	G4double fTankPosX = detectorPos.getX();
	G4double fTankPosY = detectorPos.getY();
	G4double fTankPosZ = detectorPos.getZ();
	
	// define PMT position as the center of the tank
	G4ThreeVector fTankCenter = detectorPos + G4ThreeVector(0, 0, fTankHalfHeight + fTankThickness);
	int detectorId = detector.GetId();
	int pmtId = 0;
	ostringstream namedetector;
	namedetector.str("");
	namedetector << "/SaltyWCD"+to_string(detectorId);
	cout << "[INFO] G4Models::SaltyWCD: Building detector " << namedetector.str();
	cout << " (ID = " << detectorId << ")";
	cout << " with " << pmt.GetName() << ". " << endl;
	cout << "[INFO] G4Models::SaltyWCD: Detector Dimensions:" << endl;
	cout << "Tank Radius = " << fTankRadius / CLHEP::cm << " cm " << endl;
	cout << "Tank Height = " << fTankHeight / CLHEP::cm << " cm " << endl;
	cout << "Fraction of Water Impurities = " << fNaClFracMass << endl;
	/****************************************************************
		
		Geant4 Volume construction
		
		SaltyWCD is a WCD filled with salty water. The fraction of
		NaCl w.r.t water is determined by the parameter fNaClFracMass
		which value is between 0 and 1.
		
		The water is contained in a cylinder of stainless steel.

		The PMT is simulated as a semi-sphere made of Pyrex
		to account for reflectivity of photons at the PMT
		window. The PMT volume is registered as a SensitiveDetector
		and its response is computed by the G4MPMTAction class.

	****************************************************************/
	solidCasingTop = new G4Tubs("CasingTop", 0, fTankRadius + fTankThickness, fTankThickness/2, 0, 360*deg);
	solidCasingSide = new G4Tubs("CasingSide", fTankRadius, fTankRadius + fTankThickness, fTankHalfHeight, 0, 360*deg);

	solidTank = new G4Tubs("Tank", 0, fTankRadius, fTankHalfHeight, 0, 360*deg);
	solidTop = new G4Tubs("Top", 0, fTankRadius, fTankThickness/2, 0, 360*deg);
	solidSide = new G4Tubs("Side", fTankRadius, fTankRadius + fTankThickness, fTankHalfHeight, 0, 360*deg);

	// pmt solids 
	solidPMT = new G4Ellipsoid("PMT", fPMTSemiX, fPMTSemiY, fPMTSemiZ, -fPMTSemiZ, 0);

	// assemble SaltyWCD 
	G4SDManager* const sdMan = G4SDManager::GetSDMpointer();

	// tank casing are made of Stainless-steel
	logCasingTop = new G4LogicalVolume(solidCasingTop, StainlessSteel, "logCasingTop", 0, 0, 0);
	physCasingTop = new G4PVPlacement(nullptr, G4ThreeVector(fTankPosX, fTankPosY, fTankPosZ + 2*fTankHalfHeight + 1.5*fTankThickness), logCasingTop, "physCasingTop", logMother, false, 0, fCheckOVerlaps);
	
	logCasingBot = new G4LogicalVolume(solidCasingTop, StainlessSteel, "logCasingBot", 0, 0, 0);
	physCasingBot = new G4PVPlacement(nullptr, G4ThreeVector(fTankPosX, fTankPosY, fTankPosZ + 0.5*fTankThickness), logCasingBot, "physCasingBot", logMother, false, 0);

	logCasingSide = new G4LogicalVolume(solidCasingSide, StainlessSteel, "logCasingSide", 0, 0, 0);
	physCasingSide = new G4PVPlacement(nullptr, G4ThreeVector(fTankPosX, fTankPosY, fTankPosZ + fTankHalfHeight + fTankThickness), logCasingSide, "physCasingSide", logMother, false, 0, fCheckOVerlaps);


	// water part
	logTank  = new G4LogicalVolume(solidTank, SaltyWater, "logTank", 0, 0, 0);
	physTank = new G4PVPlacement(nullptr, fTankCenter, logTank, "physTank", logMother, false, 0, fCheckOVerlaps);
	// register water logical volume in the Detector
	if (!detector.HasLogicalVolume("logTank"))
		detector.SetLogicalVolume("logTank", logTank);

	// top, bottom and side walls of the tank
	logTop  = new G4LogicalVolume(solidTop, Materials().HDPE, "logTop", 0, 0, 0);
	physTop = new G4PVPlacement(nullptr, G4ThreeVector(), logTop, "physTop", logCasingTop, false, 0, fCheckOVerlaps);
	
	logBot = new G4LogicalVolume(solidTop, Materials().HDPE, "logBot", 0, 0, 0);
	physBot = new G4PVPlacement(nullptr, G4ThreeVector(), logBot, "physBot", logCasingBot, false, 0, fCheckOVerlaps);
	
	logSide  = new G4LogicalVolume(solidSide, Materials().HDPE, "logSide", 0, 0, 0);
	physSide = new G4PVPlacement(nullptr, G4ThreeVector(), logSide, "physSide", logCasingSide, false, 0, fCheckOVerlaps);

	// tank liner surface
	new G4LogicalBorderSurface("TopSurface", physTank, physTop, Materials().LinerOptSurf);
	new G4LogicalBorderSurface("BotSurface", physTank, physBot, Materials().LinerOptSurf);
	new G4LogicalBorderSurface("SideSurface", physTank, physSide, Materials().LinerOptSurf);

	// PMT
	string logName = "logPMT_"+to_string(pmtId);
	logPMT = new G4LogicalVolume(solidPMT, Materials().Pyrex, logName, 0, 0, 0);
	new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fTankHalfHeight), logPMT, "physPMT", logTank, false, pmtId, fCheckOVerlaps);

	// register PMT in the Detector
	if (!detector.HasOptDevice(pmtId)) {
		detector.MakeOptDevice(pmtId, OptDevice::ePMT);
		cout << "[DEBUG] Adding PMT id = " << pmtId << endl;
	}
	OptDevice optDevice = detector.GetOptDevice(pmtId);
	cout << "[DEBUG] Getting " << optDevice.GetName() << " with id " << optDevice.GetId() << endl;
	// register PMT logical volume
	//if (!optDevice.HasLogicalVolume(logName))
		//optDevice.SetLogicalVolume(logName, logPMT);

	string optName = pmt.GetName() + "_"+to_string(pmtId);
	ostringstream fullName;
	fullName.str("");
	fullName << "/SaltyWCD_/"+to_string(detectorId) << "/" << optName;
	G4MPMTAction* const pmtSD = new G4MPMTAction(fullName.str().c_str(), detectorId, pmtId, theEvent);
	sdMan->AddNewDetector(pmtSD);
	logPMT->SetSensitiveDetector(pmtSD);


	// register water volume as sensitive detector
	G4MDetectorAction* const waterSD = new G4MDetectorAction(namedetector.str().c_str(), detectorId, theEvent);
	sdMan->AddNewDetector(waterSD);
	logTank->SetSensitiveDetector(waterSD);
	
}
