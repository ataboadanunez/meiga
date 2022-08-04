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