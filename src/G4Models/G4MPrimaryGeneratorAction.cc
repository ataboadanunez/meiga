// Meiga headers
#include "G4MPrimaryGeneratorAction.h"
#include "CorsikaUtilities.h"
#include "Geometry.h"

// Geant4 headers
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
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

G4MPrimaryGeneratorAction::G4MPrimaryGeneratorAction(Event& aEvent) : 
	G4VUserPrimaryGeneratorAction(),
	fParticleGun(nullptr),
	fParticleSource(nullptr),
	fEvent(aEvent),
	fPrimaryGenerator(aEvent)
	
{  
	auto inputMode = fEvent.GetSimData().GetInputMode();
	if (inputMode == SimData::InputMode::eGPS) {
		fParticleSource = new G4GeneralParticleSource();
	} else {
		fParticleGun = new G4ParticleGun(1);
	}
}

G4MPrimaryGeneratorAction::~G4MPrimaryGeneratorAction()
{
	if(fParticleGun)
		delete fParticleGun;
	if(fParticleSource)
		delete fParticleSource;
}

void 
G4MPrimaryGeneratorAction::GeneratePrimaries(G4Event* aG4Event)
{
	SimData &simData = fEvent.GetSimData();
	if (simData.GetInputMode() == SimData::InputMode::eGPS) {
		if (fParticleSource) {
			G4ParticleDefinition* pDefinition = fParticleSource->GetParticleDefinition();
			std::vector<double> pPosition = Geometry::ToStdVector(fParticleSource->GetParticlePosition());
			std::vector<double> pMomentum = Geometry::ToStdVector(fParticleSource->GetParticleMomentumDirection() * CLHEP::GeV);
			int pId = pDefinition->GetPDGEncoding();
			Particle particle = Particle(pId, pPosition, pMomentum);
			particle.SetInjectionPosition(pPosition);
			// set current particle
			simData.SetCurrentParticle(particle);
			simData.InsertParticle(particle);
			if (fEvent.GetConfig().fSaveInput) {
				simData.InsertInjectedParticle(particle);
			}
			fParticleSource->GeneratePrimaryVertex(aG4Event);
			// increase SimData event counter
			simData.IncreaseEventCounter();
			Particle currParticle = simData.GetCurrentParticle();
			PrintParticleInformation(currParticle);
		}
	} else {
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
		fParticleGun->GeneratePrimaryVertex(aG4Event);
		// increase SimData event counter
		simData.IncreaseEventCounter();
		PrintParticleInformation(currParticle);
	}
}

void G4MPrimaryGeneratorAction::PrintParticleInformation(Particle &aCurrentParticle)
{
	ostringstream msg;
	msg << "Primary Particle Information: " << endl;
	msg << "Particle name: " << aCurrentParticle.GetName() << ", ID: " << aCurrentParticle.GetParticleId() << endl;
	msg << "Particle position: (" << aCurrentParticle.GetInjectionPosition().at(0) / CLHEP::cm << ", " << aCurrentParticle.GetInjectionPosition().at(1) / CLHEP::cm << ", " << aCurrentParticle.GetInjectionPosition().at(2) / CLHEP::cm << ") cm" << endl;
	msg << "Particle direction: (" << aCurrentParticle.GetMomentumDirection().at(0) / CLHEP::GeV << ", " << aCurrentParticle.GetMomentumDirection().at(1) / CLHEP::GeV << ", " << aCurrentParticle.GetMomentumDirection().at(2) / CLHEP::GeV << ") GeV" << endl;
	msg << "Particle (Theta, Phi): (" << aCurrentParticle.GetZenith() << ", " << aCurrentParticle.GetAzimuth() << ") rad" << endl;
	msg << "Particle Momentum: " << aCurrentParticle.GetMomentum() / CLHEP::GeV << " GeV " << endl;
	msg << "Particle Kinetic Energy: " << aCurrentParticle.GetKineticEnergy() / CLHEP::GeV << " GeV " << endl;
	Logger::Print(msg, INFO, "GeneratePrimaries");
}
