// implementation of class DetectorSimData
#include "DetectorSimData.h"

void
DetectorSimData::MakeOptDeviceSimData(unsigned int id)
{
	fOptDeviceSimMap.emplace(id, OptDeviceSimData(id));
}

void 
DetectorSimData::SetEnergyDeposit(const double eDep)
{
	fEnergyDeposit.push_back(eDep);
}

void
DetectorSimData::SetEnergyDeposit(Particle::Component comp, const double eDep)
{
	auto it = fEDepComponentMap.emplace(std::make_pair(comp, double()));
	it.first->second.push_back(eDep);
}