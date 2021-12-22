// implementation of the Module class
#include "Module.h"

Module::Module() 
{
	//fModulePosition = new std::vector<double>();
	fModulePosition[3];
}	

Module::Module(unsigned int id) : fModuleId(id)
{
	//fModulePosition = new std::vector<double>();
	fModulePosition[3];
}

void
Module::MakeSiPM(unsigned int id)
{
	fSiPMMap.emplace(id, SiPM(id));
	fNSiPMs = fSiPMMap.size();
}