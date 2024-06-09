// implementation of Event class
#include "Event.h"
#include "DetectorFactory.h"
#include "Particle.h"
#include "Logger.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// Event::Event()
// {
// }

// Event::~Event()
// {
// }

bool Event::HasDetector(const int id)
{	
	return fDetectorMap.find(id) != fDetectorMap.end();
}

void Event::MakeDetector(const int id, const Detector::DetectorType type)
{
	if (type == Detector::eUnknown) {
		std::ostringstream errmsg;
		errmsg << "Unknown detector type! Possible causes of this error:\n"
				<< "- Misspelled detector type (in the DetectorList.xml).\n"
				<< "- Type not registered in the Detector class.";
		Logger::Print(errmsg, ERROR, "MakeDetector");
		
		throw std::invalid_argument("Invalid detector type");
	}
	// fDetectorMap.emplace(id, Detector(id, type));
	fDetectorMap.emplace(id, DetectorFactory::CreateDetector(id, type));
	fNDetectors = fDetectorMap.size();	
}

Detector& Event::GetDetector(const int id)
{
	if(!HasDetector(id)) {
		throw std::runtime_error("Not found detector with ID: "+id);
	}
	return *(fDetectorMap.at(id));
}