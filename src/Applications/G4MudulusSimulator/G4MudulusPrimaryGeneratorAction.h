// definition of the G4MudulusPrimaryGeneratorAction class

#ifndef G4MudulusPrimaryGeneratorAction_h
#define G4MudulusPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <iostream>

#include "G4MudulusDetectorConstruction.h"
#include "Particle.h"
#include "Event.h"

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4MudulusPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4MudulusPrimaryGeneratorAction(/*Event& theEvent, const Particle &theParticle*/);
    virtual ~G4MudulusPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    
    G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;

  private:

    G4ParticleGun* fParticleGun;

    const G4MudulusDetectorConstruction *fDetectorConstructor;

  	friend class G4MudulusDetectorConstruction;
};  

#endif
