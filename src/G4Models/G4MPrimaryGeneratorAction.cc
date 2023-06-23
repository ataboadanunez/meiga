// Meiga headers
#include "G4MPrimaryGeneratorAction.h"
#include "CorsikaUtilities.h"
#include "Geometry.h"

// Geant4 headers
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <CLHEP/Random/Randomize.h>

// C++ headers
#include <cmath>
#include <iostream>

using CLHEP::RandFlat;
using namespace std;

G4MPrimaryGeneratorAction::G4MPrimaryGeneratorAction(Event& theEvent) : 
	G4VUserPrimaryGeneratorAction(),
	fParticleGun(new G4ParticleGun(1)),
	fEvent(theEvent),
	fPrimaryGenerator(theEvent)
	
{  
	
}

G4MPrimaryGeneratorAction::~G4MPrimaryGeneratorAction()
{
	delete fParticleGun;
}


void 
G4MPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{


	SimData &simData = fEvent.GetSimData();
	// loads generation parameters to current particle in the event
	fPrimaryGenerator.GeneratePrimaryParticle();

	Particle & currParticle = simData.GetCurrentParticle();
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  unsigned int currParticleID = currParticle.GetParticleId();
	int fParticleId = Corsika::CorsikaToPDG(currParticleID);
	G4ParticleDefinition* particleDef = particleTable->FindParticle(fParticleId);

	if (!particleDef) {
		cout << "[WARNING] G4MPrimaryGeneratorAction::GeneratePrimaries: Undefined particle with Corsika ID: " << currParticleID << endl;
		return;
	}

	// pass current particle momentum (direction) and position to Geant4
	fParticleGun->SetParticleDefinition(particleDef);
	fParticleGun->SetParticleMomentumDirection(Geometry::ToG4Vector(currParticle.GetMomentumDirection(), 1.));
	fParticleGun->SetParticlePosition(Geometry::ToG4Vector(currParticle.GetPosition(), 1.));
	fParticleGun->SetParticleEnergy(currParticle.GetKineticEnergy());
	fParticleGun->GeneratePrimaryVertex(event);
  cout << "[INFO] G4MPrimaryGeneratorAction::GeneratePrimaries: Injecting particle: " << endl;
  cout << particleDef->GetParticleName() << " with ID " << currParticle.GetParticleId() << endl;
  cout << "Particle position = (" << currParticle.GetPosition().at(0) / CLHEP::cm << ", " << currParticle.GetPosition().at(1) / CLHEP::cm << ", " << currParticle.GetPosition().at(2) / CLHEP::cm << ") cm" << endl;
	cout << "Particle direction = (" << currParticle.GetMomentumDirection().at(0) / CLHEP::GeV << ", " << currParticle.GetMomentumDirection().at(1) / CLHEP::GeV << ", " << currParticle.GetMomentumDirection().at(2) / CLHEP::GeV << ") GeV" << endl;
	cout << "Particle (Theta, Phi) = (" << currParticle.GetZenith() << ", " << currParticle.GetAzimuth() << ") rad" << endl;
	cout << "Particle Momentum = " << currParticle.GetMomentum() / CLHEP::GeV << " GeV " << endl;
	cout << "Particle Kinetic Energy = " << currParticle.GetKineticEnergy() / CLHEP::GeV << " GeV " << endl;


}