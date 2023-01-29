#ifndef DefaultProperties_h
#define DefaultProperties_h 1

#include <string>


struct DefaultProperties 
{
	
	// Sets default detector porperties from configuration file
	void SetDefaultProperties(const std::string &filename = "");

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

};

#endif