// definition of the ParticleInjector class
#ifndef ParticleInjector_h
#define ParticleInjector_h 1

#include "G4ThreeVector.hh" //do we need this?? try to keep "out" of geant4, convert later to G4ThreeVector
#include <iostream>
#include <vector>
#include <string>


class ParticleInjector
{
	public:
		ParticleInjector();
		virtual ~ParticleInjector();

		void ReadParticleFile();

		G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;
    unsigned fParticleId;

	private:
    /* no need to use G4 declarations
       use std C++ here
    */
    
		std::string fFileName;
    std::ifstream* fInputStream;

		double Ro = 0;
		double initx = 0.;
		double inity = 0.;
		double initz = 0.;
		double r = 0.;
    double theta = 0.;
		double px;
		double py;
		double pz;
		double x;
		double y;
		double z;
		unsigned primaryShowerId;
		unsigned primaryParticleId;
		double 	primaryEnergy;
		double primaryTheta;
		double primaryPhi;

};
#endif
