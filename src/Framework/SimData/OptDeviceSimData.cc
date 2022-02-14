#include "OptDeviceSimData.h"

OptDeviceSimData::OptDeviceSimData(const unsigned int id) : fId(id)
{
	
	fPETimeDistribution = new std::vector<std::vector<double>*>();
}

void 
OptDeviceSimData::AddPhotonTime(const double phTime) 
{
	fPhotonTime.push_back(phTime);
}

void 
OptDeviceSimData::AddPhotonEnergy(const double phEnergy) 
{
	fPhotonEnergy.push_back(phEnergy);
}


void
OptDeviceSimData::AddPETimeDistribution(std::vector<double>* peTimeDist)
{
	fPETimeDistribution->push_back(peTimeDist);
}


std::vector<double>
OptDeviceSimData::CalculatePulse(const double fBinSize, const std::vector<double>& peTime, const double pulseLength)
{

	// case for OptDevice types is needed
	
	size_t size = pulseLength / fBinSize + 1;
	std::vector<double> result(size, 0.);
	//Fill array
	for (auto t: peTime) {
			fDetSiPM.SPEPulse(result, fBinSize, static_cast<size_t>(t/fBinSize));
	}

	return result;
}
