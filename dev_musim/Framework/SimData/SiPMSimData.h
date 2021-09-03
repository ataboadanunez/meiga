#ifndef SiPMSimData_h
#define SiPMSimData_h
#include <vector>

/*
	declaration of class SiPMSimData

	stores simulated data related to SiPM

	author: alvaro taboada
*/ 

class SiPMSimData
{
	public:
		SiPMSimData() { }
		// construct SiPMSimData given its id
		SiPMSimData(const unsigned int id); // : fId(id) { }
		virtual ~SiPMSimData() { }

		unsigned int GetId() const { return fId; }

		void AddPhotonTime(double time);
		std::vector<double> GetPhotonTime() { return fPhotonTime; }

		void AddPhotonEnergy(double energy);
		std::vector<double> GetPhotonEnergy() { return fPhotonEnergy; }


		void AddPETimeDistribution(std::vector<double>* peTimeDist);
		std::vector<std::vector<double>*>* GetPETimeDistribution() { return fPETimeDistribution; }

		//void MakeTrace(std::vector<double>* fPETime);
		//std::vector<std::pair<double, double>> GetTrace() { return fTrace; }
		
	private:

		int fId = 0;
		
		// photon quantities
		std::vector<double> fPhotonTime;
		std::vector<double> fPhotonEnergy;

		// for PE time distribution
		std::vector<std::vector<double>*>* fPETimeDistribution;
};

#endif