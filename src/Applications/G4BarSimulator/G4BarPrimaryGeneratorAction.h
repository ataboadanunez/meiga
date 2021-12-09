#ifndef G4BarPrimaryGeneratorAction_h
#define G4BarPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <iostream>

#include "G4BarDetectorConstruction.h"
#include "Particle.h"
#include "Event.h"

class G4ParticleGun;
class G4Event;

// Primary generator action class with particle gun

class G4BarPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public:
    G4BarPrimaryGeneratorAction(/*Event& theEvent, const Particle &theParticle*/);
    virtual ~G4BarPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

    double ComputeKineticEnergy(const Particle& particle);
    G4ThreeVector fParticlePosition;
    G4ThreeVector farticleDirection;

  private:

    G4ParticleGun* fParticleGun;
    
    // for testing purposes
    bool fTestEnabled = true;
    std::string fOutputFileName = "./particle_trajectories.dat"; 
    std::ofstream* fTextFile;
    
    const G4BarDetectorConstruction *fDetectorConstructor;

  	friend class G4BarDetectorConstruction;
};  

#endif
