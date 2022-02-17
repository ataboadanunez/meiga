// implementation of class SimData
#include "SimData.h"
#include "DetectorSimData.h"

using namespace std;

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

const string
SimData::GetSimulationMode() const
{
	// set wether full G4 simulation is performed or not
	if (fSimulationMode.empty()) 
		throw invalid_argument("[ERROR] Framework/SimData::GetSimulationMode: A simulator mode is needed!"); 

	return fSimulationMode;

}