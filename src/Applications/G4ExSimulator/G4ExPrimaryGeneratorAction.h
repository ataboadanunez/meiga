// definition of the G4ExPrimaryGeneratorAction class

#ifndef G4ExPrimaryGeneratorAction_h
#define G4ExPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <iostream>

#include "G4ExDetectorConstruction.h"
#include "Particle.h"
#include "Event.h"

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4ExPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4ExPrimaryGeneratorAction(/*Event& theEvent, const Particle &theParticle*/);
    virtual ~G4ExPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    
    G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;

  private:

    G4ParticleGun* fParticleGun;

    const G4ExDetectorConstruction *fDetectorConstructor;

  	friend class G4ExDetectorConstruction;
};  

#endif