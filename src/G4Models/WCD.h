#ifndef WCD_h
#define WCD_h 1

// Meiga classes
//#include "Event.h"
#include "Detector.h"
#include "Materials.h"

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
class Event;

class WCD : public Detector 
{
	// Basic Geant4 model for a Water-Cerenkov Detector
public:
	WCD(const int id, const DetectorType type);
	
	// static void BuildDetector(G4LogicalVolume* aLogMother, Detector& aDetector, Event& aEvent, G4bool aCheckOverlaps = true);
	virtual void BuildDetector(G4LogicalVolume *logMother, Event &aEvent, G4bool overlaps = false) override;
	static void ConstructSensitiveDetector(Detector &aDetector, Event &aEvent);
	
private:

	static std::ostringstream fNameDetector;
	static std::ostringstream fFullName;
	static G4LogicalVolume* fLogTank;
	static G4LogicalVolume* fLogPMT;

};

#endif