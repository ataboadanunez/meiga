// definition of the G4CasposoPrimaryGeneratorAction class

#ifndef G4CasposoPrimaryGeneratorAction_h
#define G4CasposoPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <iostream>

#include "EcoMug.h"

#include "G4CasposoDetectorConstruction.h"
#include "Particle.h"
#include "Event.h"

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4CasposoPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4CasposoPrimaryGeneratorAction(Event& theEvent);
    virtual ~G4CasposoPrimaryGeneratorAction();

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
    int fMuonCharge = 0;

  private:

    G4ParticleGun* fParticleGun;
    Event &fEvent;
    
    const G4CasposoDetectorConstruction *fDetectorConstructor;

  	friend class G4CasposoDetectorConstruction;
};  

#endif
