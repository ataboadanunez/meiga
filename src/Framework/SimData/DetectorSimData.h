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


		// OptDeviceSimData calls from Detector
		void MakeOptDeviceSimData(unsigned int id);
		OptDeviceSimData& GetOptDeviceSimData() { return fOptDeviceSimData; }
		OptDeviceSimData& GetOptDeviceSimData(unsigned int id) { return fOptDeviceSimMap[id]; }
		bool HasOptDeviceSimData(unsigned int id) { return fOptDeviceSimMap.count(id) != 0; }

		std::set<uint64_t>& GetMuonDecayID() { return fMuonDecayID; }
		const std::set<uint64_t>& GetMuonDecayID() const { return fMuonDecayID; }

		std::map<uint64_t, std::vector<uint64_t>>& GetPhotoElectronParentID() { return fPEParentID; }
		const std::map<uint64_t, std::vector<uint64_t>>& GetPhotoElectronParentID() const { return fPEParentID; }

		void SetEnergyDeposit(const double eDep);
		std::vector<double> GetEnergyDeposit() { return fEnergyDeposit; }
		// energy deposit for particle component
		void SetEnergyDeposit(Particle::Component comp, const double eDep);
		std::vector<double> GetEnergyDeposit(Particle::Component comp) { return fEDepComponentMap.at(comp); } 
		// a Has method is needed before requesting information to the map
		bool HasEnergyDeposit(Particle::Component comp) { return fEDepComponentMap.count(comp) != 0; }




	private:

		int fId = 0;
		OptDeviceSimData fOptDeviceSimData;
		std::map<int, OptDeviceSimData> fOptDeviceSimMap;

		// specific holders for muon decay analysis
		std::set<uint64_t> fMuonDecayID;
		std::map<uint64_t, std::vector<uint64_t> > fPEParentID;

		// data holders at detector level
		std::vector<double> fEnergyDeposit;
		std::map<Particle::Component, std::vector<double>> fEDepComponentMap;
};

#endif