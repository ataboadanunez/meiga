#include "Detector.h"
//#include "Module.h"

Detector::Detector() 
{
	fDetectorPosition[3];
}

Detector::Detector(unsigned int id) : fDetectorId(id) 
{
	fDetectorPosition[3];
}


// Add constructor by detector type

void
Detector::MakePixel(unsigned int id)
{
	fPixelMap.emplace(id, Pixel(id));
	fNPixels = fPixelMap.size();
}