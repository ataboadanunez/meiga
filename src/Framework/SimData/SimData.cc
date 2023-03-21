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


SimData::InputMode
SimData::InputModeConversion(string name)
{

	fInputModeName = name;

	if (name == "UseARTI")
		return SimData::InputMode::eUseARTI;
	else if (name == "UseEcoMug")
		return SimData::InputMode::eUseEcoMug;
	else {
		string what = "[ERROR] SimData::InputModeConversion: Unknown input mode: " + name;
		throw invalid_argument(what);
		return SimData::InputMode::eUnknown;
	}
}


SimData::InjectionMode
SimData::InjectionConversion(string name)
{
	
	fInjectionModeName = name;

	if (name == "eCircle")
		return SimData::InjectionMode::eCircle;
	else if (name == "eHalfSphere")
		return SimData::InjectionMode::eHalfSphere;
	else if (name == "eVertical")
		return SimData::InjectionMode::eVertical;
	else if (name == "eFromFile")
		return SimData::InjectionMode::eFromFile;
	else {
		cout << "[WARNING] SimData::InjectionConversion: Unknown injection type!" << endl;
		return SimData::InjectionMode::eUnknown;
	}
}

SimData::SimulationMode
SimData::SimulationModeConversion(string name)
{
	
	fSimulationModeName = name;

	if (name == "eFull")
		return SimData::SimulationMode::eFull;
	else if (name == "eFast")
		return SimData::SimulationMode::eFast;
	else {
		cout << "[WARNING] SimData::SimulationModeConversion: Unknown simulation type! (uwsing Full by default)." << endl;
		return SimData::SimulationMode::eFull;
	}
}

void
SimData::MakeDetectorSimData(unsigned int id)
{
	fDetectorSimDataMap.emplace(id, DetectorSimData(id));
}
