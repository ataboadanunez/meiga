// implementation of Event class
#include "Event.h"
#include "Particle.h"
#include "Logger.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


Event::Event()
{
}

Event::~Event()
{
}

void
Event::MakeDetector(const unsigned int id, const Detector::DetectorType type)
{
	if (type == Detector::eUnknown) {
		std::ostringstream errmsg;
		errmsg << "Unknown detector type! Possible causes of this error:\n"
				<< "- Misspelled detector type (in the DetectorList.xml).\n"
				<< "- Type not registered in the Detector class.";
		Logger::Print(errmsg, ERROR, "MakeDetector");
		
		throw std::invalid_argument("Invalid detector type");
	}
	fDetectorMap.emplace(id, Detector(id, type));
	fNDetectors = fDetectorMap.size();	
}

bool
Event::HasDetector(unsigned int id)
{	
	return fDetectorMap.find(id) != fDetectorMap.end();
}
