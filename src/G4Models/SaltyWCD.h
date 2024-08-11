#ifndef SaltyWCD_h
#define SaltyWCD_h 1

// Meiga classes
#include "Event.h"
#include "Detector.h"
#include "Materials.h"
#include "WCD.h"

// geant4 classes
// #include "G4SDManager.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Ellipsoid.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;

class SaltyWCD : public WCD
{
	// Basic Geant4 model for a NaCl-Water-Cerenkov Detector
public:
	SaltyWCD(const int aId, const Detector::DetectorType aType);
	void BuildDetector(G4LogicalVolume* logMother, Event& theEvent, G4bool fCheckOverlaps = true) override;

private:
	// virtual void SetDefaultProperties() override;

private:
	static std::ostringstream fNameDetector;
	static std::ostringstream fFullName;
	static G4LogicalVolume* fLogTank;
	static G4LogicalVolume* fLogPMT;
};

#endif