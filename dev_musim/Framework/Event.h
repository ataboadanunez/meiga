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
    
    Event();

    typedef std::vector<Particle> ParticleVector;
    const ParticleVector& GetParticleVector() const { return fParticles; }
    void InsertParticle(const Particle& particle) { fParticles.push_back(particle); }

    //mover a SimData
    unsigned int GetTotalNumberOfParticles() const { return fTotalNumberOfParticles; }
    void SetTotalNumberOfParticles(const unsigned int n) { fTotalNumberOfParticles = n; }

    // por ahora aqui. eventualmente sera un metodo de la clase SimData 
    // y tomara como entrada el id del SiPM donde los fotones fueron detectados
    void AddPhotonTime(const double peTime);
    std::vector<double> GetPhotonTime() { return fPETimes; }

  private:

    ParticleVector fParticles;
    int fTotalNumberOfParticles = 0;
    std::vector<double> fPETimes;

};




#endif
