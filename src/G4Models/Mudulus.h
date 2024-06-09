#ifndef Mudulus_h
#define Mudulus_h 1

// #include "G4SDManager.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"

#include "Event.h"
#include "Detector.h"
#include "Materials.h"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;

class Mudulus : public Detector
{
	// an hodoscope-like muon detector with SiPM in both of the fiber endpoints.

public:
	Mudulus(const int aId, const Detector::DetectorType aType);
	void BuildDetector(G4LogicalVolume* logMother, Event& theEvent, G4bool fCheckOverlaps = false) override;

	static G4double GetFiberXCoordinate(G4int barId, G4double barLength, G4double fiberLength);



};

#endif
