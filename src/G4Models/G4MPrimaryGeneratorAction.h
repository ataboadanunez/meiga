// definition of the G4MPrimaryGeneratorAction class
#ifndef G4MPrimaryGeneratorAction_h
#define G4MPrimaryGeneratorAction_h 1

// Geant4 headers
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

// Meiga headers
#include "PrimaryGenerator.h"
#include "Particle.h"
#include "Event.h"

class G4ParticleGun;
class G4Event;

// Model for a primary generator action class with particle gun

class G4MPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4MPrimaryGeneratorAction(Event& theEvent);
    virtual ~G4MPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    
    PrimaryGenerator fPrimaryGenerator;
    
  private:

    G4ParticleGun* fParticleGun;
    Event& fEvent;
    
};  

#endif
