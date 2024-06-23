#ifndef Hodoscope_h
#define Hodoscope_h 1

#include "G4LogicalVolume.hh"

#include "Event.h"
#include "Detector.h"
#include "Scintillator.h"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;

class Hodoscope : public Scintillator
{
	/*
		Model class for an hodoscope-like detector.
		This type of detectors are used in muon detection techniques.
		
		Consists of a given number of planes composed by scintillator bars.
		Scintillator bars are displayed along the x- and y-axis conforming
		grids of (2N_x - 1) * (2N_y - 1) number of pixels per plane.


	*/

public:
	Hodoscope(const int id, const Detector::DetectorType type);
	void BuildDetector(G4LogicalVolume* logMother, Event& aEvent, G4bool fCheckOverlaps = false) override;

};

#endif