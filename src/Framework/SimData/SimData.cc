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

SimData::InjectionMode
SimData::InjectionConversion(string name)
{
	
	fInjectionModeName = name;

	if (name == "eRandom")
		return SimData::eRandom;
	else if (name == "ePositionFromFile")
		return SimData::ePositionFromFile;
	else if (name == "eInsideDetector")
		return SimData::eInsideDetector;
	else {
		cout << "[WARNING] SimData::StringToType: Unknown injection type!" << endl;
		return SimData::eUnknown;
	}
}

SimData::SimulationMode
SimData::ModeConversion(string name)
{
	
	fSimulationModeName = name;

	if (name == "eFull")
		return SimData::eFull;
	else if (name == "eFast")
		return SimData::eFast;
	else {
		cout << "[WARNING] SimData::StringToType: Unknown simulation type! (uwsing Full by default)." << endl;
		return SimData::eFull;
	}
}

void
SimData::MakeDetectorSimData(unsigned int id)
{
	fDetectorSimDataMap.emplace(id, DetectorSimData(id));
}
