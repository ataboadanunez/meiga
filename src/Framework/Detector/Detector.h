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

		// Module class getters.
		/*
			Module is a detector composed by panels of scintillator bars,
			WLS fibers and SiPMs / PMTs (Musaic / Mudulus)
		*/
		void MakeModule(unsigned int id);
    Module& GetModule() { return fModule; }
    Module& GetModule(unsigned int id) { return fModuleMap[id]; }
		 
		int GetNModules() const { return fNModules; }
		
		std::map<int, Module>& ModulesRange() { return fModuleMap; }
		const std::map<int, Module>& ModulesRange() const { return fModuleMap; }

		// WCD ?

	private:

		int fNModules = 0;
		Module fModule;
		std::map<int, Module> fModuleMap;

};

#endif
