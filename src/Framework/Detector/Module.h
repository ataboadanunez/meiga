// definition of Module class
#ifndef Module_h
#define Module_h 1

#include <vector>
#include <map>

#include "SiPM.h"

class Module {
	
	// Module detector class
	// author: alvaro taboada

	public:
		Module();
		Module(const unsigned int id);
		virtual ~Module() { }

		// module properties
		int GetId() const { return fModuleId; }
		void SetId(int id) { fModuleId = id; }
		//bool ModuleExists() { return fModuleId; } 

		// std::vector<double>* GetModulePosition() { return fModulePosition; }
		// void SetModulePosition(std::vector<double>* pos) { fModulePosition = pos; }

    std::vector<double> GetModulePosition() { return fModulePosition; }
    void SetModulePosition(std::vector<double> pos) { fModulePosition = pos; }

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

		void MakeSiPM(unsigned int id);
		SiPM& GetSiPM() { return fSiPM; }
		SiPM& GetSiPM(unsigned int id) { return fSiPMMap[id]; }

		std::map<int, SiPM>& SiPMsRange() { return fSiPMMap; }
		const std::map<int, SiPM>& SiPMsRange() const { return fSiPMMap; }

	private:
		// base detector properties
		// the way properties are set now is redundant
		// need to implement this in a config file and set from there using a manager
		// AS: usar boost::ptree. lee archivos .json y .xml
		// https://www.technical-recipes.com/2014/using-boostproperty_tree/
		// escribir codigo en Framework/ConfigManager que lea todo eso


		int fModuleId = 0;
		int fNBars = 0;
		int fNSiPMs = 0;

		// scintillator bar dimensions
    double fBarWidth = 41;
    double fBarLength = 82;
    double fBarThickness = 10;
    double fCasingThickness = 2;
    // fiber dimensions
    double fFiberRadius = 1.2;
    double fCladdingThickness = 0.10;
    double fCoatingThickness  = 0.25;

    //std::vector<double>* fModulePosition;
    std::vector<double> fModulePosition;
    std::map<int, SiPM> fSiPMMap;
    SiPM fSiPM;
};


#endif
