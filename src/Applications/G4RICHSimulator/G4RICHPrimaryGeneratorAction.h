// definition of the G4RICHPrimaryGeneratorAction class

#ifndef G4RICHPrimaryGeneratorAction_h
#define G4RICHPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include "G4RICHConstruction.h"
#include "Particle.h"
#include "Event.h"

#include <iostream>

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4RICHPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4RICHPrimaryGeneratorAction(Event& theEvent);
    virtual ~G4RICHPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    
    G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;

  private:

    G4ParticleGun* fParticleGun;

    const G4RICHConstruction *fDetectorConstructor;

    friend class G4RICHConstruction;
    Event& fEvent;
};  

#endif
