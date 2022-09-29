#include <iostream>

#include "Dummy.h"
#include "Geometry.h"

using namespace std;

void
Dummy::BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOverlaps)
{
	
	int detectorId = detector.GetId();
	G4ThreeVector detectorPos = Geometry::ToG4Vector(detector.GetDetectorPosition(), 1.);

	ostringstream namedetector;
	namedetector.str("");
	namedetector << "Dummy";
	cout << "[INFO] G4Models::Dummy: Building detector " << namedetector.str();
	cout << "(ID = " << detectorId << ")" << endl;
	cout << "[INFO] G4Models::Dummy: Detector Position = ";
	cout << "(" << detectorPos.getX() / CLHEP::cm << ", " << detectorPos.getY() << ", " << detectorPos.getZ() << ") " << endl;



}