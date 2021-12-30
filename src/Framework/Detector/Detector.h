#ifndef Detector_h
#define Detector_h

//#include "Module.h"
//#include "G4SystemOfUnits.hh"
#include "Pixel.h"
#include <vector>
#include <map>


/*
	declaration of Detector class

	contains methods for the definition of the detector components
	this class is intended to set all these parameters which then
	will be taken by the DetectorConstructor class in Geant4.

	a detector can be composed of several modules, and each module
	is composed by its enclosure, scintillator bars, WLS fibers, SiPMs, etc.

	all methods and functions related to the detector geometry and components
	should be written here and not in the Geant4 code. 

	author: alvaro taboada

*/

class Detector
{
	public:
		Detector();
		Detector(const unsigned int id); 
		virtual ~Detector() { }

		int GetId() const { return fDetectorId; }
		void SetId(int id) { fDetectorId = id; }

		std::vector<double> GetDetectorPosition() { return fDetectorPosition; }
		void SetDetectorPosition(std::vector<double> pos) { fDetectorPosition = pos; }

		// Mechanical properties
		double GetCasingThickness() const { return fCasingThickness; }
		void SetCasingThickness(double casingThickness) { fCasingThickness = casingThickness; }
		double GetHeight() const { return fHeight; }
		void SetHeight(double h) { fHeight = h; }
		double GetRadius() const { return fRadius; }
		void SetRadius(double r) { fRadius = r; }

		// scintillator-type detector properties
		int GetNBars() const { return fNBars; }
		void SetNBars(int nBars) { fNBars = nBars; }

		double GetBarLength() const { return fBarLength; }
		void SetBarLength(double barLength) { fBarLength = barLength; }
		
		double GetBarWidth() const { return fBarWidth; }
		void SetBarWidth(double barWidth) { fBarWidth = barWidth; }
		
		double GetBarThickness() const { return fBarThickness; }
		void SetBarThickness(double barThickness) { fBarThickness = barThickness; }
		
		double GetBarCoatingThickness() const { return fCoatingThickness; }
		void SetBarCoatingThickness(double barCoatThickness) { fCoatingThickness = barCoatThickness; }

		// WLS fibers
		double GetFiberRadius() const { return fFiberRadius; }
		void SetFiberRadius(double fiberRad) { fFiberRadius = fiberRad; }

		double GetCladdingThickness() const { return fCladdingThickness; }
		void SetCladdingThickness(double cladThickness) { fCladdingThickness = cladThickness; }

		// Pixel

		void MakePixel(unsigned int id);
		Pixel& GetPixel() { return fPixel; }
		Pixel& GetPixel(unsigned int id) { return fPixelMap[id]; }

		std::map<unsigned int, Pixel>& PixelRange() { return fPixelMap; }
		const std::map<unsigned int, Pixel>& PixelRange() const { return fPixelMap; }

		//void MakeModule(unsigned int id);
		//Module& GetModule() { return fModule; }
		//Module& GetModule(unsigned int id) { return fModuleMap[id]; }
		 
		//int GetNModules() const { return fNModules; }
		
		
		// WCD ?

	private:

		int fDetectorId = 0;
		int fNBars = 0;
		int fNPixels = 0;

		double fBarWidth = 41;
		double fBarLength = 82;
		double fBarThickness = 10;
		double fCasingThickness = 2;
		double fFiberRadius = 1.2;
		double fCladdingThickness = 0.10;
		double fCoatingThickness  = 0.25;

		double fHeight = 0;
		double fRadius = 0;
		

		//std::vector<double>* fModulePosition;
		std::vector<double> fDetectorPosition;
		std::map<unsigned int, Pixel> fPixelMap;
		Pixel fPixel;
		//std::map<int, Module> fModuleMap;

};

#endif
