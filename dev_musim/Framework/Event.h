#ifndef Event_h
#define Event_h 1

#include "Particle.h"
#include <vector>
#include <string>

/**
	MuSim Event Class
	
	This class is intended to store methods and variables
	that will be handled during the simulation / reconstruction
	processes. 

	todo: Description about "event" concept.

	author: Alvaro Taboada
	date: 16 June 2021

	$Id:$
*/


class Event 
{
	public:
		Event() { fParticles.clear(); }
		static Event ReadParticleFile(const std::string &fileName);

		typedef std::vector<Particle> ParticleVector;
		const ParticleVector& GetParticleVector() const { return fParticles; }
		void InsertParticle(const Particle& particle) { fParticles.push_back(particle); }
		
		unsigned int GetTotalNumberOfParticles() const { return fTotalNumberOfParticles; }
		void SetTotalNumberOfParticles(const unsigned int n) { fTotalNumberOfParticles = n; }
		
	private:

		ParticleVector fParticles;
		int fTotalNumberOfParticles = 0;
};




#endif
