// definition of the G4RockPrimaryGeneratorAction class

#ifndef G4RockPrimaryGeneratorAction_h
#define G4RockPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <iostream>

#include "Event.h"

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4RockPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4RockPrimaryGeneratorAction(/*Event& theEvent, const Particle &theParticle*/);
    virtual ~G4RockPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

    G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;

    //void setParticle(const Particle &p) { fParticle = p; }

    unsigned fParticleId;
    double fPx;
    double fPy;
    double fPz;

  private:

    G4ParticleGun* fParticleGun;
    
    //Event &fEvent;
    //const Particle &fParticle;

    // for testing purposes
    bool fTestEnabled = true;
    std::string fOutputFileName = "/home/alvaro/data/particle_trajectories.dat"; 
    std::ofstream* fTextFile;
};  

#endif
