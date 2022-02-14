#ifndef DetectorSimData_h
#define DetectorSimData_h
#include "OptDeviceSimData.h"
#include <vector>
#include <map>

/*

	class DetectorSimData
	contains methods for storing and computing simulated data
	related to the detector


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
		void MakeOptDeviceSimData(unsigned int id);
		OptDeviceSimData& GetOptDeviceSimData() { return fOptDeviceSimData; }
		OptDeviceSimData& GetOptDeviceSimData(unsigned int id) { return fOptDeviceSimMap[id]; }

	private:

		int fId = 0;
		OptDeviceSimData fOptDeviceSimData;
		std::map<int, OptDeviceSimData> fOptDeviceSimMap;


};

#endif