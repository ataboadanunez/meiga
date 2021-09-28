#ifndef DetectorSimData_h
#define DetectorSimData_h
#include "SiPMSimData.h"
#include <vector>
#include <map>

/*
	declaration of class DetectorSimData

	stores simulated data related to modules.
	modules are composed by scintillator bars,
	WLS fibers, SiPMs...

	author: alvaro taboada

*/

class DetectorSimData
{
	public:
		DetectorSimData() { }
		//constructor given module Id
		DetectorSimData(const unsigned int id) : fId(id) { }
		virtual ~DetectorSimData() { }

		unsigned int GetModuleId() const { return fId; }

		// SiPMSimData calls from Detector
		void MakeSiPMSimData(unsigned int id);
		SiPMSimData& GetSiPMSimData() { return fSiPMSimData; }
		SiPMSimData& GetSiPMSimData(unsigned int id) { return fSiPMSimMap[id]; }

	private:

		int fId = 0;
		SiPMSimData fSiPMSimData;
		std::map<int, SiPMSimData> fSiPMSimMap;


};

#endif