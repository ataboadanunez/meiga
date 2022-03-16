#ifndef Scintillator_h
#define Scintillator_h 1

#include "G4SDManager.hh"
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

class Scintillator {
	// basic scintillator detector

	public:

		static void BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOverlaps = true);

	private:
		Scintillator();
		virtual ~Scintillator();


};

#endif
