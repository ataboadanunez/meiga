#ifndef DefaultProperties_h
#define DefaultProperties_h 1

#include <string>

// path to DetectorProperties.xml is set from a global variable from CMakeLists.txt
// this path will be overwritten after compilation
#define DETECTORPROPERTIES "/home/alvaro/projects/github/meiga/build/Framework/ConfigManager/DetectorProperties.xml"

struct DefaultProperties 
{
	
	// Sets default detector porperties from configuration
	
	void SetDefaultProperties();

	double gBarWidth;
	double gBarLength;
	double gBarThickness;
	double gCasingThickness;
	double gRotationAngle;
	double gFiberLength;
	double gFiberRadius;
	double gCladdingThickness;
	double gCoatingThickness;
	int    gNumberOfBars;
	int    gNumberOfPanels;
	double gTankHeight;
	double gTankRadius;
	double gTankThickness;
	double gLength;
	double gWidth;
	double gDistancePanels;
	double gThickness;
	double gGroundSizeX;
	double gGroundSizeY;
	double gGroundSizeZ;
	// fraction of impurities for detectors in water
	double gImpuritiesFraction;

};

#endif
