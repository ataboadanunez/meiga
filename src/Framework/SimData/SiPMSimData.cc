// implementation of class SiPMSimData
#include "SiPMSimData.h"

SiPMSimData::SiPMSimData(const unsigned int id) : fId(id)
{
	
	fPETimeDistribution = new std::vector<std::vector<double>*>();
}

void 
SiPMSimData::AddPhotonTime(const double phTime) 
{
	fPhotonTime.push_back(phTime);
}

void 
SiPMSimData::AddPhotonEnergy(const double phEnergy) 
{
	fPhotonEnergy.push_back(phEnergy);
}


void
SiPMSimData::AddPETimeDistribution(std::vector<double>* peTimeDist)
{
	fPETimeDistribution->push_back(peTimeDist);
}


std::vector<double>
SiPMSimData::CalculatePulse(const double fBinSize, const std::vector<double>& peTime, const double pulseLength)
{

	// double pulseDuration = fDetSiPM.GetSPEPulseDuration();
	// Calculate array size: Start of last pulse + pulse duration in bins
	// AT: fixed pulse size
	//size_t size = (*std::max_element(peTime.begin(), peTime.end()) + pulseDuration)/fBinSize + 1;
	size_t size = pulseLength / fBinSize + 1;
	std::vector<double> result(size, 0.);
	//Fill array
	for (auto t: peTime) {
			fDetSiPM.SPEPulse(result, fBinSize, static_cast<size_t>(t/fBinSize));
		}

	return result;
}
