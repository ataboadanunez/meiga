#ifndef Dummy_h
#define Dummy_h 1

// Meiga classes
#include "Event.h"
#include "Detector.h"
#include "Materials.h"

// geant4 classes
#include "G4SDManager.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Box.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;

class Dummy {
	// class for a Dummy detector
	
public:
	static void BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOverlaps = true);
	
private:
	Dummy();
	virtual ~Dummy();

};

#endif