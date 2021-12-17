#include "Detector.h"
#include "Module.h"

Detector::Detector() 
{
	
}

void
Detector::MakeModule(unsigned int id)
{
	fModuleMap.emplace(id, Module(id));
	fNModules = fModuleMap.size();
}

