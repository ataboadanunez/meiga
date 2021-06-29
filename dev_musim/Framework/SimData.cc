// implementation of class SimData
#include "SimData.h"

SimData::SimData()
{
	fParticles.clear();
}

SimData::~SimData()
{

}

void 
SimData::AddPhotonTime(const double peTime) 
{
  fPETimes.push_back(peTime);
}