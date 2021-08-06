#ifndef Detector_h
#define Detector_h

#include "Module.h"
#include "G4SystemOfUnits.hh"
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
		virtual ~Detector() { }

		// get Module class
		void MakeModule(unsigned int id);
    Module& GetModule() { return fModule; }
    Module& GetModule(unsigned int id) { return fModuleMap[id]; }
		
		// crear vector de objetos Module? 
		int GetNModules() const { return fNModules; }
		void SetNModules(int nMod) { fNModules = nMod; }


	private:

		int fNModules = 0;
		Module fModule;
		std::map<int, Module> fModuleMap;

};

#endif
