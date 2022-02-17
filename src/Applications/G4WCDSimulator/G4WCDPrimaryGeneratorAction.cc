// implementation of the G4WCDPrimaryGeneratorAction class
#include "CorsikaUtilities.h"
#include "Geometry.h"

#include "G4WCDSimulator.h"
#include "G4WCDPrimaryGeneratorAction.h"
#include "G4WCDConstruction.h"


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

G4WCDPrimaryGeneratorAction::G4WCDPrimaryGeneratorAction(/*Event& theEvent, const Particle &theParticle*/) : 
  G4VUserPrimaryGeneratorAction(),
  fParticleGun(new G4ParticleGun(1))
  
{  

	fDetectorConstructor = static_cast<const G4WCDConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  

}

G4WCDPrimaryGeneratorAction::~G4WCDPrimaryGeneratorAction()
{
  delete fParticleGun;
}


void 
G4WCDPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  
  // get particle information
  Particle& currParticle = G4WCDSimulator::currentParticle;
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  unsigned int currParticleID = currParticle.GetParticleId();
  int fParticleId = Corsika::CorsikaToPDG(currParticleID);
  G4ParticleDefinition* particleDef = particleTable->FindParticle(fParticleId);

  if (!particleDef) {
      cout << "[WARNING] G4WCDPrimaryGeneratorAction::GeneratePrimaries: Undefined particle with Corsika ID: " << currParticleID << endl;
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
  const G4double injRadius = 105 * CLHEP::cm;
  const G4double injHeight = 90 * CLHEP::cm + 10*CLHEP::cm; // slightly above tank  
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
  //fParticleGun->SetParticlePosition(G4ThreeVector(1*cm, 1*cm, 1*m));
  fParticleGun->SetParticleEnergy(fKineticEnergy);
  fParticleGun->GeneratePrimaryVertex(event);
  
}