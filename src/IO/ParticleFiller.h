#ifndef ParticleFiller_h
#define ParticleFiller_h 1

#include "Event.h"
#include <string>

/* 
	Declaration of ParticleFiller class.
	This class is intended to hold methods / functions 
	related to read the input file with particles information.

	Author: Alvaro Taboada
*/

class Event;

class ParticleFiller 
{
	public:
		ParticleFiller();
		virtual ~ParticleFiller();

		static void FillParticleVector(const std::string &fileName, Event& theEvent);
		static void FillParticleVector(const unsigned int &numberOfParticles, Event& theEvent);



};

#endif