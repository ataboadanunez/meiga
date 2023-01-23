// definition of the G4HodoscopePrimaryGeneratorAction class

#ifndef G4HodoscopePrimaryGeneratorAction_h
#define G4HodoscopePrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <iostream>

#include "EcoMug.h"

#include "G4HodoscopeDetectorConstruction.h"
#include "Particle.h"
#include "Event.h"

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4HodoscopePrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4HodoscopePrimaryGeneratorAction(Event& theEvent);
    virtual ~G4HodoscopePrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    // for EcoMug muon generation
    G4ParticleDefinition *mu_plus; 
    G4ParticleDefinition *mu_minus;
    
    G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;

    EcoMug fMuonGen;
    bool fUseEcoMug = true;
    SimData::InjectionMode injMode;
    int fMuonCharge = 0;

  private:

    G4ParticleGun* fParticleGun;
    Event &fEvent;
    
    const G4HodoscopeDetectorConstruction *fDetectorConstructor;

  	friend class G4HodoscopeDetectorConstruction;
};  

#endif
