// definition of the G4AndesPrimaryGeneratorAction class

#ifndef G4AndesPrimaryGeneratorAction_h
#define G4AndesPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <iostream>

#include "G4AndesDetectorConstruction.h"
#include "Particle.h"
#include "Event.h"

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4AndesPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4AndesPrimaryGeneratorAction(Event& theEvent);
    virtual ~G4AndesPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    
    G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;

  private:

    G4ParticleGun* fParticleGun;
    Event &fEvent;
    
    const G4AndesDetectorConstruction *fDetectorConstructor;

  	friend class G4AndesDetectorConstruction;
};  

#endif
