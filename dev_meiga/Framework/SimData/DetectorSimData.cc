// implementation of class DetectorSimData
#include "DetectorSimData.h"

// DetectorSimData::DetectorSimData()
// {
// 	//fSiPMVector.clear()
// }

// DetectorSimData::~DetectorSimData()
// {

// }

void
DetectorSimData::MakeSiPMSimData(unsigned int id)
{
	fSiPMSimMap.emplace(id, SiPMSimData(id));
}