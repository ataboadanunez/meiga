#ifndef Hodoscope_h
#define Hodoscope_h 1

#include "G4LogicalVolume.hh"

#include "Event.h"
#include "Detector.h"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;

class Hodoscope {
	/*
		Model class for an hodoscope-like detector.
		This type of detectors are used in muon detection techniques.
		
		Consists of a given number of planes composed by scintillator bars.
		Scintillator bars are displayed along the x- and y-axis conforming
		grids of (2N_x - 1) * (2N_y - 1) number of pixels per plane.


	*/

public:
	static void BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOverlaps = false);

private:
	Hodoscope();
	virtual ~Hodoscope();


};

#endif