#ifndef DetectorSimData_h
#define DetectorSimData_h
#include "OptDeviceSimData.h"
#include <vector>
#include <map>
#include <set>
#include <cstdint>

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

		std::set<uint64_t>& GetMuonDecayID() { return fMuonDecayID; }
		const std::set<uint64_t>& GetMuonDecayID() const { return fMuonDecayID; }

		std::map<uint64_t, std::vector<uint64_t>>& GetPhotoElectronParentID() { return fPEParentID; }
		const std::map<uint64_t, std::vector<uint64_t>>& GetPhotoElectronParentID() const { return fPEParentID; }

	private:

		int fId = 0;
		OptDeviceSimData fOptDeviceSimData;
		std::map<int, OptDeviceSimData> fOptDeviceSimMap;

		std::set<uint64_t> fMuonDecayID;
		std::map<uint64_t, std::vector<uint64_t> > fPEParentID;
};

#endif