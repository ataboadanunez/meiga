#ifndef Detector_h
#define Detector_h

#include "G4SystemOfUnits.hh"
#include <vector>

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
		virtual ~Detector() { }

		// module properties
		int GetModuleId() const { return fModuleId; }
		void SetModuleId(int id) { fModuleId = id+1; }
		bool ModuleExists() { return fModuleId; } 

		std::vector<double>* GetModulePosition() { return fModulePosition; }
		void SetModulePosition(std::vector<double>* pos) { fModulePosition = pos; }

		int GetNModules() const { return fNModules; }
		void SetNModules(int nMod) { fNModules = nMod; }

		double GetCasingThickness() const { return fCasingThickness; }
		void SetCasingThickness(double casingThickness) { fCasingThickness = casingThickness; }

		// scintillator bar properties
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


	private:

		// base detector properties
		// the way properties are set now is redundant
		// need to implement this in a config file and set from there using a manager
		// adrian: usar boost::ptree. lee archivos .json y .xml
		// https://www.technical-recipes.com/2014/using-boostproperty_tree/
		// escribir codigo en Framework/ConfigManager que lea todo eso


		int fModuleId = 0;
		int fNModules = 0;
		int fNBars = 0;

    double fBarWidth = 41;
    double fBarLength = 2*fBarWidth;
    double fBarThickness = 10;
    double fFiberRadius = 1.2;
    double fCasingThickness = 2;
    double fCladdingThickness = 0.10;
    double fCoatingThickness  = 0.25;
    double fSiPMSizeX = 1.*mm;
    double fSiPMSizeY = 1.*mm; 
    double fSiPMSizeZ = 0.1*mm;

    std::vector<double>* fModulePosition;

};

#endif
