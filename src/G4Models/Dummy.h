#ifndef Dummy_h
#define Dummy_h 1

// Meiga classes
#include "Event.h"
#include "Detector.h"
#include "Materials.h"

// geant4 classes
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Box.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;

class Dummy : public Detector
{

	// class for a Dummy detector
	
public:
	Dummy(const int aId, const Detector::DetectorType aType);
	void BuildDetector(G4LogicalVolume* logMother, Event& theEvent, G4bool fCheckOverlaps = true) override;

};

#endif