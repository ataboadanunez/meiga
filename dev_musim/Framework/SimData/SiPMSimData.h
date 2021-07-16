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
		SiPMSimData(const unsigned int id) : fId(id) { }
		virtual ~SiPMSimData() { }

		unsigned int GetId() const { return fId; }
		void AddPhotonTime(double time);
		std::vector<double> GetPhotonTime() { return fPETimes; }
		
	private:
		int fId = 0;
		std::vector<double> fPETimes;
};

#endif