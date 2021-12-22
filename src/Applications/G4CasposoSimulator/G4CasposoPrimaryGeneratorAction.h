// definition of the G4CasposoPrimaryGeneratorAction class

#ifndef G4CasposoPrimaryGeneratorAction_h
#define G4CasposoPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <iostream>

#include "G4CasposoDetectorConstruction.h"
#include "Particle.h"
#include "Event.h"

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4CasposoPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4CasposoPrimaryGeneratorAction(/*Event& theEvent, const Particle &theParticle*/);
    virtual ~G4CasposoPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    
    G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;

  private:

    G4ParticleGun* fParticleGun;

    const G4CasposoDetectorConstruction *fDetectorConstructor;

  	friend class G4CasposoDetectorConstruction;
};  

#endif