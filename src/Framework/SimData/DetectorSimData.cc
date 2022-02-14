// implementation of class DetectorSimData
#include "DetectorSimData.h"

void
DetectorSimData::MakeOptDeviceSimData(unsigned int id)
{
	fOptDeviceSimMap.emplace(id, OptDeviceSimData(id));
}