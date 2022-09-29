// definition of the G4NeutronPrimaryGeneratorAction class

#ifndef G4NeutronPrimaryGeneratorAction_h
#define G4NeutronPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <iostream>

#include "G4NeutronDetectorConstruction.h"
#include "Particle.h"
#include "Event.h"

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4NeutronPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4NeutronPrimaryGeneratorAction(Event& theEvent);
    virtual ~G4NeutronPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    
    G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;

  private:

    G4ParticleGun* fParticleGun;
    Event& fEvent;
    const G4NeutronDetectorConstruction *fDetectorConstructor;

  	friend class G4NeutronDetectorConstruction;
};  

#endif
