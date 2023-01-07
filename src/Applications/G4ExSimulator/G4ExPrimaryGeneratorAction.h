// definition of the G4ExPrimaryGeneratorAction class

#ifndef G4ExPrimaryGeneratorAction_h
#define G4ExPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <iostream>

#include "EcoMug.h"

#include "G4ExDetectorConstruction.h"
#include "Particle.h"
#include "Event.h"

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4ExPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4ExPrimaryGeneratorAction(Event& theEvent);
    virtual ~G4ExPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    
    G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;
    // for EcoMug muon generation
    G4ParticleDefinition *mu_plus; 
    G4ParticleDefinition *mu_minus;
    EcoMug fMuonGen;
    int fMuonCharge = 0;
    bool fUseEcoMug = false;
  private:

    G4ParticleGun* fParticleGun;
    Event& fEvent;
    const G4ExDetectorConstruction *fDetectorConstructor;

  	friend class G4ExDetectorConstruction;
};  

#endif
