// implementation of the Module class
#include "Module.h"

Module::Module() 
{
	fModulePosition = new std::vector<double>();
}

Module::Module(unsigned int id) : fModuleId(id)
{
	fModulePosition = new std::vector<double>();
}