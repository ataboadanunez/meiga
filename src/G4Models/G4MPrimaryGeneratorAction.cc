// Meiga headers
#include "G4MPrimaryGeneratorAction.h"
#include "CorsikaUtilities.h"
#include "Geometry.h"

// Geant4 headers
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
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
	
	// Particle ID is its PDG Encoding 
	const auto fParticleId = currParticle.GetParticleId();
	
	G4ParticleDefinition* particleDef = nullptr;
	if (currParticle.IsNucleus()) {

		const auto z = currParticle.GetAtomicNumber();
		const auto a = currParticle.GetMassNumber();
	
		particleDef = G4IonTable::GetIonTable()->GetIon(z, a, 0.);
	
	} else {

		particleDef = particleTable->FindParticle(currParticle.GetName());

	}

	if (!particleDef) {
		ostringstream msg;
		msg << "Undefined particle: " << currParticle.GetName() << " (" << currParticle.GetParticleId() << ") ";
		Logger::Print(msg, WARNING, "GeneratePrimaryParticle");
	}

	// set current particle momentum (direction) and position to Geant4
	fParticleGun->SetParticleDefinition(particleDef);
	fParticleGun->SetParticleMomentumDirection(Geometry::ToG4Vector(currParticle.GetMomentumDirection(), 1.));
	fParticleGun->SetParticlePosition(Geometry::ToG4Vector(currParticle.GetInjectionPosition(), 1.));
	fParticleGun->SetParticleEnergy(currParticle.GetKineticEnergy());
	fParticleGun->GeneratePrimaryVertex(event);

	ostringstream msg;
	msg << "Primary Particle Information: " << endl;
	msg << "Particle name: " << particleDef->GetParticleName() << ", ID: " << particleDef->GetPDGEncoding() << endl;
	msg << "Particle position: (" << currParticle.GetInjectionPosition().at(0) / CLHEP::cm << ", " << currParticle.GetInjectionPosition().at(1) / CLHEP::cm << ", " << currParticle.GetInjectionPosition().at(2) / CLHEP::cm << ") cm" << endl;
	msg << "Particle direction: (" << currParticle.GetMomentumDirection().at(0) / CLHEP::GeV << ", " << currParticle.GetMomentumDirection().at(1) / CLHEP::GeV << ", " << currParticle.GetMomentumDirection().at(2) / CLHEP::GeV << ") GeV" << endl;
	msg << "Particle (Theta, Phi): (" << currParticle.GetZenith() << ", " << currParticle.GetAzimuth() << ") rad" << endl;
	msg << "Particle Momentum: " << currParticle.GetMomentum() / CLHEP::GeV << " GeV " << endl;
	msg << "Particle Kinetic Energy: " << currParticle.GetKineticEnergy() / CLHEP::GeV << " GeV " << endl;
	Logger::Print(msg, INFO, "GeneratePrimaries");
}