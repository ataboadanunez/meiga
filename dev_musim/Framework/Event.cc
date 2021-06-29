// implementation of Event class
#include "Event.h"
#include "Particle.h"
#include <iostream>
#include <fstream>
#include <sstream>


Event::Event()
{
  fParticles.clear();
}

void 
Event::AddPhotonTime(const double peTime) 
{
  fPETimes.push_back(peTime);
}