// implementation of the G4RockPrimaryGeneratorAction class

#include "G4RockSimulator.h"
#include "G4RockPrimaryGeneratorAction.h"
#include "CorsikaUtilities.h"

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

G4RockPrimaryGeneratorAction::G4RockPrimaryGeneratorAction(/*Event& theEvent, const Particle &theParticle*/) : 
  //G4VUserPrimaryGeneratorAction(),
  fParticleGun(new G4ParticleGun(1))
  //,fEvent(theEvent)
  //,fParticle(theParticle)
  /*,
  fParticleInjector(partInj)*/
{  
  if (fTestEnabled) {
    fTextFile = new ofstream(fOutputFileName.c_str());
  }

  // write case for injection from macro, not from file of secondaries
  
  /*
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle = particleTable->FindParticle(particleName="mu+"); // de momento lo dejamos asi
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,-1.,0.));
  fParticleGun->SetParticleEnergy(10.*GeV);
  */
}

G4RockPrimaryGeneratorAction::~G4RockPrimaryGeneratorAction()
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
G4RockPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  
  // get particle information
  const Particle& currParticle = G4RockSimulator::currentParticle;
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  unsigned int currParticleID = currParticle.GetParticleId();
  int fParticleId = Corsika::CorsikaToPDG(currParticleID);
  G4ParticleDefinition* particleDef = particleTable->FindParticle(fParticleId);

  if (!particleDef) {
      cout << "[WARNING] G4RockPrimaryGeneratorAction::GeneratePrimaries: Undefined particle with Corsika ID: " << currParticleID << endl;
      return;
    }
  
  double mass = currParticle.GetMass();
  const double kineticEnergy = ComputeKineticEnergy(currParticle);
  double fMomentum = currParticle.GetMomentum() * GeV;
  const std::vector<double> particleMomentumDirection = currParticle.GetParticleDirection();
  cout << "Vector Size = " << particleMomentumDirection.size() << endl;
 	double fPx = particleMomentumDirection.at(0);
  double fPy = particleMomentumDirection.at(1);
  double fPz = particleMomentumDirection.at(2);
  //double fPx = currParticle.GetParticleMomentumX();
  //double fPy = currParticle.GetParticleMomentumY();
  //double fPz = currParticle.GetParticleMomentumZ();
  // fix momentum units
  fPx *= GeV; fPy *= GeV; fPz *= GeV;
 
  fParticleGun->SetParticleDefinition(particleDef);

  G4double px2 = fPx*fPx;
  G4double py2 = fPy*fPy;
  G4double pz2 = fPz*fPz;

  G4double particleMomentum = sqrt(px2 + py2 + pz2);
  G4double particleZenith = acos(fPz / particleMomentum);
  G4double particleAzimut = atan2(-fPy, -fPx);

  // 
  /* inject particles following distributions according their incoming direction
    for now, we inject in a circle over the scintillator
    randomized on top of the scintillator
    x = [-Barwidth/2, BarWidth/2]
    z = [-BarLength/2, BarLength/2]
    y = fixed height
  */
 
  G4double BarWidth = 41*mm;
  G4double BarLength = 2*BarWidth;

#warning "compute injection radius according to module size!"

  const G4double injRadius = 2.5*BarWidth;
  const G4double injHeight = 5*cm;    
  const G4double rand = RandFlat::shoot(0., 1.);
  const G4double r = injRadius*sqrt(rand);
  const G4double phi = RandFlat::shoot(0., M_2_PI);

  G4double x0 = r*cos(phi) - BarWidth/2;
  G4double y0 = injHeight + 3*m;
  G4double z0 = r*sin(phi);
  
  G4cout << "[DEBUG] G4RockPrimaryGeneratorAction " << currParticleID << " " << " " << particleDef->GetParticleName() << " " << mass / GeV << " " << fMomentum / GeV << " " << particleMomentum / GeV << " " << kineticEnergy / GeV << G4endl;
  if (fTestEnabled) {
  	/*
			Intended to test copy of MuSim classes to G4
			Particle information, energy calculation, position, etc
  	*/
    *fTextFile << currParticleID << " " << mass << " " << fPx << " " << fPy << " " << fPz << " " << endl; 
  }

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(fPx, -1*fPz, fPy));
  fParticleGun->SetParticlePosition(G4ThreeVector(0., y0, 0.));
  fParticleGun->SetParticleEnergy(kineticEnergy);

  fParticleGun->GeneratePrimaryVertex(event);
}


double
G4RockPrimaryGeneratorAction::ComputeKineticEnergy(const Particle& particle)
{	
	// caveat: mass are expressed in MeV (see Framework/PhysicsConstants.h)
	double mass = particle.GetMass() / GeV;
	double p = particle.GetMomentum() * GeV;

	double k = sqrt(p*p + mass*mass) - mass;
	// should be given already in GeV
	return k;
}