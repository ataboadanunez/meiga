#include <iostream>

#include "WCD.h"
#include "Geometry.h"
#include "G4MDetectorAction.h"
#include "G4MPMTAction.h"

#include "G4SDManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

using namespace std;

G4LogicalVolume *WCD::fLogTank = nullptr;
G4LogicalVolume *WCD::fLogPMT = nullptr;
std::ostringstream WCD::fNameDetector;
std::ostringstream WCD::fFullName;

void WCD::BuildDetector(G4LogicalVolume *aLogMother, Detector &aDetector, Event &aEvent, G4bool aCheckOVerlaps)
{
	// solids
	G4Tubs* solidBot  = nullptr;

	// WCD dimensions
	G4double fTankRadius = aDetector.GetTankRadius();
	G4double fTankHeight = aDetector.GetTankHeight();
	G4double fTankHalfHeight = 0.5 * fTankHeight;
	G4double fTankThickness = aDetector.GetTankThickness();
	
	// PMT properties photonis-XP1805
	OptDevice pmt = aDetector.GetOptDevice(OptDevice::ePMT);
	G4double fPMTSemiX = pmt.GetSemiAxisX() * CLHEP::cm;
	G4double fPMTSemiY = pmt.GetSemiAxisY() * CLHEP::cm;
	G4double fPMTSemiZ = pmt.GetSemiAxisZ() * CLHEP::cm;

	G4ThreeVector detectorPos = Geometry::ToG4Vector(aDetector.GetDetectorPosition(), 1.);
	G4double fTankPosX = detectorPos.getX();
	G4double fTankPosY = detectorPos.getY();
	G4double fTankPosZ = detectorPos.getZ();
	
	// define PMT position as the center of the tank
	G4ThreeVector fTankCenter = detectorPos + G4ThreeVector(0, 0, fTankHalfHeight + fTankThickness);
	int detectorId = aDetector.GetId();
	int pmtId = 0;
	fNameDetector.str("");
	fNameDetector << "/WCD_"+to_string(detectorId);
	cout << "[INFO] G4Models::WCD: Building detector " << fNameDetector.str();
	cout << " (ID = " << detectorId << ")";
	cout << " with " << pmt.GetName() << ". " << endl;
	cout << "[INFO] G4Models::WCD: Detector Dimensions:" << endl;
	cout << "Tank Radius = " << fTankRadius / CLHEP::cm << " cm " << endl;
	cout << "Tank Height = " << fTankHeight / CLHEP::cm << " cm " << endl;
	/****************************************************************
		
		Geant4 Volume construction
		
		The Water-Cerenkov Detector (WCD) is a cylindrical tank filled
		of water. The water is contained in a liner bag made of HDPE
		(High Density Polyethylene) and it contains a large 
		photo-multiplier tube at the top of the liner for collecting 
		the Cerenkov photons produced by charged particles in water.
		
		The PMT is simulated as a semi-sphere made of Pyrex
		to account for reflectivity of photons at the PMT
		window. The PMT volume is registered as a SensitiveDetector
		and its response is computed by the G4MPMTAction class.

	****************************************************************/
	// Tank
	G4Tubs* solidTank = new G4Tubs("Tank", 0, fTankRadius, fTankHalfHeight, 0, 360*deg);
	G4Tubs* solidTop = new G4Tubs("Top", 0, fTankRadius + fTankThickness, fTankThickness/2, 0, 360*deg);
	G4Tubs* solidSide = new G4Tubs("Side", fTankRadius, fTankRadius + fTankThickness, fTankHalfHeight, 0, 360*deg);

	// water part
	fLogTank  = new G4LogicalVolume(solidTank, Materials().Water, "logTank", 0, 0, 0);
	G4PVPlacement* physTank = new G4PVPlacement(nullptr, fTankCenter, fLogTank, "physTank", aLogMother, false, 0, aCheckOVerlaps);
	// register water logical volume in the Detector
	if (!aDetector.HasLogicalVolume("logTank"))
		aDetector.SetLogicalVolume("logTank", fLogTank);
	
	// top, bottom and side walls of the tank
	G4LogicalVolume* logTop  = new G4LogicalVolume(solidTop, Materials().HDPE, "logTop", 0, 0, 0);
	G4PVPlacement* physTop = new G4PVPlacement(nullptr, 
												G4ThreeVector(fTankPosX, fTankPosY, fTankPosZ + 2*fTankHalfHeight + 1.5*fTankThickness), 
												logTop, "physTop", aLogMother, 
												false, 0, aCheckOVerlaps);
	// solidBot = nullptr??
	G4LogicalVolume* logBot = new G4LogicalVolume(solidBot, Materials().HDPE, "logBot", 0, 0, 0);
	G4PVPlacement* physBot = new G4PVPlacement(nullptr, 
												G4ThreeVector(fTankPosX, fTankPosY, fTankPosZ + 0.5*fTankThickness), 
												logTop, "physBot", aLogMother, 
												false, 0, aCheckOVerlaps);
	
	G4LogicalVolume* logSide  = new G4LogicalVolume(solidSide, Materials().HDPE, "logSide", 0, 0, 0);
	G4PVPlacement* physSide = new G4PVPlacement(nullptr, 
												G4ThreeVector(fTankPosX, fTankPosY, fTankPosZ + fTankHalfHeight + fTankThickness), logSide, "physSide", aLogMother, 
												false, 0, aCheckOVerlaps);

	// tank liner surface
	new G4LogicalBorderSurface("TopSurface", physTank, physTop, Materials().LinerOptSurf);
	new G4LogicalBorderSurface("BotSurface", physTank, physBot, Materials().LinerOptSurf);
	new G4LogicalBorderSurface("SideSurface", physTank, physSide, Materials().LinerOptSurf);

	// PMT
	G4Ellipsoid* solidPMT = new G4Ellipsoid("PMT", fPMTSemiX, fPMTSemiY, fPMTSemiZ, -fPMTSemiZ, 0);
	string logName = "logPMT_"+to_string(pmtId);
	fLogPMT = new G4LogicalVolume(solidPMT, Materials().Pyrex, logName, 0, 0, 0);
	new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fTankHalfHeight), fLogPMT, "physPMT", fLogTank, false, pmtId, aCheckOVerlaps);

	// register PMT in the Detector
	if (!aDetector.HasOptDevice(pmtId)) {
		aDetector.MakeOptDevice(pmtId, OptDevice::ePMT);
		cout << "[INFO] Adding PMT id = " << pmtId << endl;
	}
	OptDevice optDevice = aDetector.GetOptDevice(pmtId);
	string optName = pmt.GetName() + "_"+to_string(pmtId);
	fFullName.str("");
	fFullName << "/WCD_"+to_string(detectorId) << "/" << optName;

	// // register PMT as sensitive detector	
	// G4MPMTAction* const pmtSD = new G4MPMTAction(fullName.str().c_str(), detectorId, pmtId, aEvent);
	// sdMan->AddNewDetector(pmtSD);
	// logPMT->SetSensitiveDetector(pmtSD);

	// // register water volume as sensitive detector
	// G4MDetectorAction* const waterSD = new G4MDetectorAction(namedetector.str().c_str(), detectorId, aEvent);
	// sdMan->AddNewDetector(waterSD);
	// logTank->SetSensitiveDetector(waterSD);
	
}

void WCD::ConstructSensitiveDetector(Detector &aDetector, Event &aEvent)
{

	auto sdMan = G4SDManager::GetSDMpointer();
	// register PMT as sensitive detector	
	G4MPMTAction* const pmtSD = new G4MPMTAction(fFullName.str().c_str(), aDetector.GetId(), 0, aEvent);
	sdMan->AddNewDetector(pmtSD);
	if(fLogPMT != nullptr) {
		fLogPMT->SetSensitiveDetector(pmtSD);
	}
	// register water volume as sensitive detector
	G4MDetectorAction* const waterSD = new G4MDetectorAction(fNameDetector.str().c_str(), aDetector.GetId(), aEvent);
	sdMan->AddNewDetector(waterSD);
	if(fLogTank != nullptr) {
		fLogTank->SetSensitiveDetector(waterSD);
	}

}
