#ifndef SiPMSimData_h
#define SiPMSimData_h
#include <vector>
#include <algorithm>
#include <cmath>
#include "SiPM.h"

/*
	declaration of class SiPMSimData

	stores simulated data related to SiPM

	author: alvaro taboada
*/ 

/*
static const double SPEDURATION = 100.;

#warning MOVER ESTA FUNCION A UN .CC
static void SPEPulse(std::vector<double> &dest, double fBinTime, size_t startPulse) {
		static const double A1 = 0.3, A2 = 23.8, A3 = 1.59;

		static const double tr = 3.9, tf1 = 1.13, tf2 = 22.2, tf3 = 2.51e-1;

		const size_t bins = (SPEDURATION/fBinTime);

		for (size_t i = 0; i < bins && i < dest.size(); ++i) {
			const double t = i*fBinTime;
			dest[startPulse + i] += A1 * (1.-exp(-t / tr)) * (A2*exp(-t/tf1) + A3*exp(-t/tf2) + exp(-t/tf3));
		}
}

*/


class SiPMSimData
{
	public:
		SiPMSimData() { }
		// construct SiPMSimData given its id
		SiPMSimData(const unsigned int id); // : fId(id) { }
		virtual ~SiPMSimData() { }

		unsigned int GetId() const { return fId; }

		void AddPhotonTime(const double time);
		std::vector<double> GetPhotonTime() { return fPhotonTime; }

		void AddPhotonEnergy(const double energy);
		std::vector<double> GetPhotonEnergy() { return fPhotonEnergy; }


		void AddPETimeDistribution(std::vector<double>* peTimeDist);
		std::vector<std::vector<double>*>* PETimeDistributionRange() { return fPETimeDistribution; }

		std::vector<double> CalculatePulse(const double fBinSize, const std::vector<double>& peTime, const double pulseLenght = 500.);
		/*
		{
			//Calculate array size: Start of last pulse + pulse duration in bins
			size_t size = (*std::max_element(peTime.begin(), peTime.end()) + SPEDURATION)/fBinSize + 1;
			std::vector<double> res(size, 0.);
			//Fill array
			for (auto t: peTime) {
				SPEPulse(res, fBinSize, static_cast<size_t>(t/fBinSize));
			}

			return res;
		}
		*/
		
	private:

		int fId = 0;
		
		// photon quantities
		std::vector<double> fPhotonTime;
		std::vector<double> fPhotonEnergy;

		// for PE time distribution
		std::vector<std::vector<double>*>* fPETimeDistribution;

		// for SiPM pulses
		std::vector<std::pair<double, double>> fTrace;
		
		SiPM fDetSiPM;

};

#endif
