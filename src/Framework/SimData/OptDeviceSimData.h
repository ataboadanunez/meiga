#ifndef OptDeviceSimData_h
#define OptDeviceSimData_h
#include <vector>
#include <algorithm>
#include <cmath>
#include "SiPM.h"
#include "Particle.h"

/*
	class OptDeviceSimData

	contains methods for storing and computing simulated data
	of any optical device (SiPM, MultiChannel PMT and PMT)


	author: alvaro taboada
*/ 



class OptDeviceSimData
{
	public:
		OptDeviceSimData() { }
		// construct OptDeviceSimData given its id
		OptDeviceSimData(const unsigned int id); // : fId(id) { }
		virtual ~OptDeviceSimData() { }

		unsigned int GetId() const { return fId; }

		void AddPhotonTime(const double time);
		std::vector<double> GetPhotonTime() { return fPhotonTime; }

		void AddPhotonEnergy(const double energy);
		std::vector<double> GetPhotonEnergy() { return fPhotonEnergy; }

		// photo-electron time distribution
		void AddPETimeDistribution(std::vector<double>* peTimeDist);
		std::vector<std::vector<double>*>* PETimeDistributionRange() { return fPETimeDistribution; }
		
		// for particle types
		void AddPETimeDistribution(Particle::Type type, const std::vector<double>& peTimeDist);
		const std::vector<std::vector<double>>& PETimeDistributionRange(Particle::Type type) { return particleTypeMap.at(type); }
		bool HasPETimeDistribution(Particle::Type type) const { return !particleTypeMap.count(type); }

		// for particle components
		void AddPETimeDistribution(Particle::Component comp, const std::vector<double>& peTimeDist);
		const std::vector<std::vector<double>>& PETimeDistributionRange(Particle::Component comp) { return particleCompMap.at(comp); }
		bool HasPETimeDistribution(Particle::Component comp) const { return particleCompMap.count(comp); }
		

		// charge (number of photo-electrons) holders needed


		// for SiPM pulse calculation.
		// this function needs to be generalized for each type of optical device
		std::vector<double> CalculatePulse(const double fBinSize, const std::vector<double>& peTime, const double pulseLenght = 500.);

	private:

		int fId = 0;
		
		// photon quantities
		std::vector<double> fPhotonTime;
		std::vector<double> fPhotonEnergy;

		// for PE time distribution
		std::vector<std::vector<double>*>* fPETimeDistribution;
		// add PE time distribution for different particle types
		std::map<Particle::Type, std::vector<std::vector<double>> > particleTypeMap;
		// add PE time distribution for different particle components
		std::map<Particle::Component, std::vector<std::vector<double>> > particleCompMap;
		// access to detector members
		SiPM fDetSiPM;
		// PMT, MChPMT

};

#endif
