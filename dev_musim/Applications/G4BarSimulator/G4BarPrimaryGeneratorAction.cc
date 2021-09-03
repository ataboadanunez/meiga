#include "CorsikaUtilities.h"
#include "Geometry.h"

#include "G4BarSimulator.h"
#include "G4BarPrimaryGeneratorAction.h"
#include "G4BarDetectorConstruction.h"


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

G4BarPrimaryGeneratorAction::G4BarPrimaryGeneratorAction(/*Event& theEvent, const Particle &theParticle*/) : 
  G4VUserPrimaryGeneratorAction(),
  fParticleGun(new G4ParticleGun(1))
  
{  

	fDetectorConstructor = static_cast<const G4BarDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  
  if (fTestEnabled) {
    fTextFile = new ofstream(fOutputFileName.c_str());
  }

}

G4BarPrimaryGeneratorAction::~G4BarPrimaryGeneratorAction()
{
  delete fParticleGun;

  if (fTestEnabled) {
    delete fTextFile;
  }
}

//static void printParticle(const Particle &p) {
//  std::cout << "DEBUG: Particle ID " << p.getID() << std::endl;
//}

void 
G4BarPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  
  // get particle information
  Particle& currParticle = G4BarSimulator::currentParticle;
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  unsigned int currParticleID = currParticle.GetParticleId();
  int fParticleId = Corsika::CorsikaToPDG(currParticleID);
  G4ParticleDefinition* particleDef = particleTable->FindParticle(fParticleId);

  if (!particleDef) {
      cout << "[WARNING] G4BarPrimaryGeneratorAction::GeneratePrimaries: Undefined particle with Corsika ID: " << currParticleID << endl;
      return;
    }
  
  double mass = currParticle.GetMass();
  const double kineticEnergy = ComputeKineticEnergy(currParticle);
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

  const G4double injRadius = 50*mm;//rockRadius;
  const G4double injHeight = 10*cm;//rockDepth + 10*cm; // few cm above  
  const G4double rand = RandFlat::shoot(0., 1.);
  const G4double r = injRadius*sqrt(rand);
  const G4double phi = RandFlat::shoot(0., CLHEP::twopi);

  G4double x0 = r*cos(phi);
  G4double y0 = r*sin(phi);
  G4double z0 = injHeight;
  
  const std::vector<double> injectionPosition = {x0, y0, z0};
  currParticle.SetInjectionPosition(injectionPosition);

  //G4cout << "[DEBUG] G4BarPrimaryGeneratorAction: Injected Position = " << x0 / m << " " << y0 / m << " " << z0 / m << G4endl;

  //x0 = 0;
  //y0 = 15*mm;

  if (fTestEnabled) {
  	/*
			Intended to test copy of MuSim classes to G4
			Particle information, energy calculation, position, etc
  	*/
    *fTextFile << currParticleID << " " << mass << " " << x0 / cm << " " << y0 / cm << " " << z0 / cm << " " << endl; 
  }
  
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(fPx, fPy, -1*fPz));
  fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
  fParticleGun->SetParticleEnergy(fKineticEnergy);

  fParticleGun->GeneratePrimaryVertex(event);
}


double
G4BarPrimaryGeneratorAction::ComputeKineticEnergy(const Particle& particle)
{	
	// caveat: mass are expressed in MeV (see Framework/PhysicsConstants.h)
	double mass = particle.GetMass();
	double p = particle.GetMomentum();

	double k = (sqrt(p*p + mass*mass) - mass);
	// should be given already in GeV
	return k;
}