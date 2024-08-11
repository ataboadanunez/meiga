// implementation of class SimData
#include "SimData.h"
#include "DetectorSimData.h"
#include "Logger.h"

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
		ostringstream msg;
		msg << "Unknown input mode: " + name;
		Logger::Print(msg, ERROR, "InputModeConversion");
		throw invalid_argument("Invalid input mode");
	}
	return SimData::InputMode::eUnknown;
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
		Logger::Print("Unknown injection type!", ERROR, "InjectionConversion");
		throw invalid_argument("Invalid injection type.");
	}
	return SimData::InjectionMode::eUnknown;
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
		Logger::Print("Unknown simulation type! (uwsing Full by default).", WARNING, "SimulationModeConversion");
		return SimData::SimulationMode::eFull;
	}
}

void
SimData::MakeDetectorSimData(unsigned int id)
{
	fDetectorSimDataMap.emplace(id, DetectorSimData(id));
}
