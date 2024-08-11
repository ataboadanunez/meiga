#include <iostream>

#include "Dummy.h"
#include "Geometry.h"

using namespace std;


Dummy::Dummy(const int aId, const Detector::DetectorType aType) :
	Detector(aId, aType)
{
	fName = "Dummy";
}


void
Dummy::BuildDetector(G4LogicalVolume* logMother, Event& , G4bool )
{
	
	int detectorId = GetId();
	G4ThreeVector detectorPos = Geometry::ToG4Vector(GetDetectorPosition(), 1.);

	ostringstream namedetector;
	namedetector.str("");
	namedetector << "Dummy";
	cout << "[INFO] G4Models::Dummy: Building detector " << namedetector.str() << " (ID = " << detectorId << ")" << endl;
	cout << "[INFO] G4Models::Dummy: Detector Position = ";
	cout << "(" << detectorPos.getX() / CLHEP::cm << ", " << detectorPos.getY() << ", " << detectorPos.getZ() / CLHEP::cm << ") cm" << endl;



}
