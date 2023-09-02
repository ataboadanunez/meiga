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
		const OptDeviceSimData& GetOptDeviceSimData() const  { return fOptDeviceSimData; }
		OptDeviceSimData& GetOptDeviceSimData(unsigned int id) { return fOptDeviceSimMap[id]; }
		const OptDeviceSimData& GetOptDeviceSimData(unsigned int id) const  { return fOptDeviceSimMap.at(id); }
		bool HasOptDeviceSimData(unsigned int id) const { return fOptDeviceSimMap.count(id) != 0; }

		std::set<uint64_t>& GetMuonDecayID() { return fMuonDecayID; }
		const std::set<uint64_t>& GetMuonDecayID() const { return fMuonDecayID; }

		std::map<uint64_t, std::vector<uint64_t>>& GetPhotoElectronParentID() { return fPEParentID; }
		const std::map<uint64_t, std::vector<uint64_t>>& GetPhotoElectronParentID() const { return fPEParentID; }

		// methods to extract deposited energy in a detector
		void SetEnergyDeposit(const double eDep);
		std::vector<double> GetEnergyDeposit() { return fEnergyDeposit; }
		const std::vector<double>& GetEnergyDeposit() const { return fEnergyDeposit; }
		// energy deposit for particle component
		void SetEnergyDeposit(Particle::Component comp, const double eDep);
		std::vector<double> GetEnergyDeposit(Particle::Component comp) { return fEDepComponentMap.at(comp); } 
		const std::vector<double>& GetEnergyDeposit(Particle::Component comp) const { return fEDepComponentMap.at(comp); } 
		// a Has method is needed before requesting information to the map
		bool HasEnergyDeposit() const { return !fEnergyDeposit.empty(); }
		bool HasEnergyDeposit(Particle::Component comp) const { return fEDepComponentMap.count(comp) != 0; }

		// methods to compute deposited energy at a single event
		void AddEnergyDeposit(const double e) { fTotalEnergyDeposit += e; }
		// reset deposited energy counter on a single event
		void ClearTotalEnergyDeposit() { fTotalEnergyDeposit = 0; }
		// Total deposited energy on a single event
		double GetTotalEnergyDeposit() const { return fTotalEnergyDeposit; }

		// bar hits for scintillator detectors
		void AddHitBarIndex(const int barId) { fHitBarIndices.push_back(barId); }
		const std::vector<int> GetHitBarIndices() const { return fHitBarIndices; }
		void ClearHitBarIndices() { fHitBarIndices.clear(); }

		void AddBinaryCounter(const std::string binaryString) { fBinaryCounter.push_back(binaryString); }
		const std::vector<std::string>& GetBinaryCounter() const { return fBinaryCounter; }
		bool HasBinaryCounter() const { return !fBinaryCounter.empty(); }


	private:

		int fId = 0;
		OptDeviceSimData fOptDeviceSimData;
		std::map<int, OptDeviceSimData> fOptDeviceSimMap;

		// specific holders for muon decay analysis
		std::set<uint64_t> fMuonDecayID;
		std::map<uint64_t, std::vector<uint64_t> > fPEParentID;

		// data holders at detector level

		// vectors to store deposited energy per event
		std::vector<double> fEnergyDeposit;
		std::map<Particle::Component, std::vector<double>> fEDepComponentMap;

		// total deposited energy at a single event
		double fTotalEnergyDeposit;

		// vectors to store hits at scintillator bars
		std::vector<int> fHitBarIndices;
		std::vector<std::string> fBinaryCounter;
};

#endif