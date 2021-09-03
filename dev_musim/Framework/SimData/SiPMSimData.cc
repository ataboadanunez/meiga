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