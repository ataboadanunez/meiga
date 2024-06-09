#ifndef Scintillator_h
#define Scintillator_h 1

// #include "G4SDManager.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"

#include "Detector.h"
#include "Materials.h"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;
class Event;

class Scintillator : public Detector
{
	// model class for a basic scintillator detector

	public:
		Scintillator(const int id, const Detector::DetectorType type);
		virtual void BuildDetector(G4LogicalVolume *logMother, Event& aEvent, G4bool overlaps = false) override;
};

#endif
