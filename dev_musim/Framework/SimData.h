#ifndef SimData_h
#define SimData_h

#include "Particle.h"
#include <vector>



class SimData
{
	public:
		SimData();
		~SimData();

		typedef std::vector<Particle> ParticleVector;
    const ParticleVector& GetParticleVector() const { return fParticles; }
    void InsertParticle(const Particle& particle) { fParticles.push_back(particle); }

		unsigned int GetTotalNumberOfParticles() const { return fTotalNumberOfParticles; }
  	void SetTotalNumberOfParticles(const unsigned int n) { fTotalNumberOfParticles = n; }

  	void AddPhotonTime(const double peTime);
    std::vector<double> GetPhotonTime() { return fPETimes; }


  private:

  	ParticleVector fParticles;
  	unsigned int fTotalNumberOfParticles = 0;
    std::vector<double> fPETimes;

};

#endif