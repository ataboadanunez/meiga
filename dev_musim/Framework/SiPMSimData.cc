// implementation of class SiPMSimData
#include "SiPMSimData.h"

void 
SiPMSimData::AddPhotonTime(const double peTime) 
{
  fPETimes.push_back(peTime);
}