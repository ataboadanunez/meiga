// implementation of class SimData
#include "SimData.h"
#include "DetectorSimData.h"

SimData::SimData()
{
	fParticles.clear();
}

SimData::~SimData()
{

}

void
SimData::MakeDetectorSimData(unsigned int id)
{
	fDetectorSimDataMap.emplace(id, DetectorSimData(id));
}