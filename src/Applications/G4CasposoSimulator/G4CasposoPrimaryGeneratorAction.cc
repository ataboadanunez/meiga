// implementation of the G4CasposoPrimaryGeneratorAction class
#include "CorsikaUtilities.h"
#include "Geometry.h"

#include "G4CasposoSimulator.h"
#include "G4CasposoPrimaryGeneratorAction.h"
#include "G4CasposoDetectorConstruction.h"


#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include <CLHEP/Random/Randomize.h>
#include <cmath>
#include <iostream>

using CLHEP::RandFlat;
using namespace std;

G4CasposoPrimaryGeneratorAction::G4CasposoPrimaryGeneratorAction(/*Event& theEvent, const Particle &theParticle*/) : 
  G4VUserPrimaryGeneratorAction(),
  fParticleGun(new G4ParticleGun(1))
  
{  

	fDetectorConstructor = static_cast<const G4CasposoDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  

}

G4CasposoPrimaryGeneratorAction::~G4CasposoPrimaryGeneratorAction()
{
  delete fParticleGun;
}


void 
G4CasposoPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  
  // get particle information
  Particle& currParticle = G4CasposoSimulator::currentParticle;
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  unsigned int currParticleID = currParticle.GetParticleId();
  int fParticleId = Corsika::CorsikaToPDG(currParticleID);
  G4ParticleDefinition* particleDef = particleTable->FindParticle(fParticleId);

  if (!particleDef) {
      cout << "[WARNING] G4CasposoPrimaryGeneratorAction::GeneratePrimaries: Undefined particle with Corsika ID: " << currParticleID << endl;
      return;
    }
  
  double mass = currParticle.GetMass();
  double fMomentum = currParticle.GetMomentum();
  double fKineticEnergy = currParticle.GetKineticEnergy();

  const std::vector<double> particleMomentumDirection = currParticle.GetDirection();
  
 	double fPx = particleMomentumDirection.at(0);
  double fPy = particleMomentumDirection.at(1);
  double fPz = particleMomentumDirection.at(2);
  
  fParticleGun->SetParticleDefinition(particleDef);

  G4double px2 = fPx*fPx;
  G4double py2 = fPy*fPy;
  G4double pz2 = fPz*fPz;

  G4double particleMomentum = sqrt(px2 + py2 + pz2);
  G4double particleZenith = acos(fPz / particleMomentum);
  G4double particleAzimut = atan2(-fPy, -fPx);

  currParticle.SetAzimuth(particleAzimut);
  currParticle.SetZenith(particleZenith);

  // check injection according to detector dimensions
  const G4double injRadius = 0.5*m;
  const G4double injHeight = 10*cm;  
  const G4double rand = RandFlat::shoot(0., 1.);
  const G4double r = injRadius*sqrt(rand);
  const G4double phi = RandFlat::shoot(0., CLHEP::twopi);

  G4double x0 = r*cos(phi);
  G4double y0 = r*sin(phi);
  G4double z0 = injHeight;
  
  const std::vector<double> injectionPosition = {x0, y0, z0};
  currParticle.SetInjectionPosition(injectionPosition);
  
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(fPx, fPy, -1*fPz));
  fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
  fParticleGun->SetParticleEnergy(fKineticEnergy);

  fParticleGun->GeneratePrimaryVertex(event);
}
