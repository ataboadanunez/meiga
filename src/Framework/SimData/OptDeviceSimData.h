#ifndef OptDeviceSimData_h
#define OptDeviceSimData_h
#include <vector>
#include <algorithm>
#include <cmath>
#include "SiPM.h"

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


		void AddPETimeDistribution(std::vector<double>* peTimeDist);
		std::vector<std::vector<double>*>* PETimeDistributionRange() { return fPETimeDistribution; }

		std::vector<double> CalculatePulse(const double fBinSize, const std::vector<double>& peTime, const double pulseLenght = 500.);
		
	private:

		int fId = 0;
		
		// photon quantities
		std::vector<double> fPhotonTime;
		std::vector<double> fPhotonEnergy;

		// for PE time distribution
		std::vector<std::vector<double>*>* fPETimeDistribution;
		// add PE time distribution for different components

		// access to detector members
		SiPM fDetSiPM;
		// PMT, MChPMT

};

#endif
