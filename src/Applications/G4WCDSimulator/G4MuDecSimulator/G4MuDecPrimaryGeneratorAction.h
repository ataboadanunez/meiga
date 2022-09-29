// definition of the G4MuDecPrimaryGeneratorAction class

#ifndef G4MuDecPrimaryGeneratorAction_h
#define G4MuDecPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include "G4MuDecConstruction.h"
#include "Particle.h"
#include "Event.h"

#include <iostream>

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4MuDecPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4MuDecPrimaryGeneratorAction(Event& theEvent);
    virtual ~G4MuDecPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    
    G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;

  private:

    G4ParticleGun* fParticleGun;

    const G4MuDecConstruction *fDetectorConstructor;

    friend class G4MuDecConstruction;
    Event& fEvent;
};  

#endif
