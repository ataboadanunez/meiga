// definition of the G4WCDPrimaryGeneratorAction class

#ifndef G4WCDPrimaryGeneratorAction_h
#define G4WCDPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include "G4WCDConstruction.h"
#include "Particle.h"
#include "Event.h"

#include <iostream>

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4WCDPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4WCDPrimaryGeneratorAction(Event& theEvent);
    virtual ~G4WCDPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    
    G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;

  private:

    G4ParticleGun* fParticleGun;

    const G4WCDConstruction *fDetectorConstructor;

    friend class G4WCDConstruction;
    Event& fEvent;
};  

#endif
