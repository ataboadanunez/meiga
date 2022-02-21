// implementation of Event class
#include "Event.h"
#include "Particle.h"
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
	fDetectorMap.emplace(id, Detector(id, type));
	fNDetectors = fDetectorMap.size();
}

bool
Event::HasDetector(unsigned int id)
{	

	return fDetectorMap.find(id) != fDetectorMap.end();
}
