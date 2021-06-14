// definition of the G4RockPrimaryGeneratorAction class

#ifndef G4RockPrimaryGeneratorAction_h
#define G4RockPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <iostream>

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4RockPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
	public:
		G4RockPrimaryGeneratorAction(/*ParticleInjector* partInj*/);
		virtual ~G4RockPrimaryGeneratorAction();

		// method from the base class
		virtual void GeneratePrimaries(G4Event*);

		void ReadParticleFile();
		
		// method to access particle gun
		const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

		G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;
    unsigned fParticleId;

	private:

		G4ParticleGun* fParticleGun;
		//ParticleInjector* fParticleInjector;

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

		// for testing purposes
		bool fTestEnabled = true;
		std::string fOutputFileName = "/home/alvaro/data/particle_trajectories.dat"; 
		std::ofstream* fTextFile;

};	

#endif
