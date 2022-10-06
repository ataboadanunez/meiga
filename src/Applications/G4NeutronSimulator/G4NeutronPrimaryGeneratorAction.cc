// implementation of the G4NeutronPrimaryGeneratorAction class
#include "CorsikaUtilities.h"
#include "Geometry.h"

#include "G4NeutronSimulator.h"
#include "G4NeutronPrimaryGeneratorAction.h"
#include "G4NeutronDetectorConstruction.h"


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

G4NeutronPrimaryGeneratorAction::G4NeutronPrimaryGeneratorAction(Event& theEvent) : 
	G4VUserPrimaryGeneratorAction(),
	fParticleGun(new G4ParticleGun(1)),
	fEvent(theEvent)
	
{  

	fDetectorConstructor = static_cast<const G4NeutronDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	

}

G4NeutronPrimaryGeneratorAction::~G4NeutronPrimaryGeneratorAction()
{
	delete fParticleGun;
}


void 
G4NeutronPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	
	// get particle information
	Particle& currParticle = G4NeutronSimulator::currentParticle;
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

	unsigned int currParticleID = currParticle.GetParticleId();
	int fParticleId = Corsika::CorsikaToPDG(currParticleID);
	G4ParticleDefinition* particleDef = particleTable->FindParticle(fParticleId);

	if (!particleDef) {
			cout << "[WARNING] G4NeutronPrimaryGeneratorAction::GeneratePrimaries: Undefined particle with Corsika ID: " << currParticleID << endl;
			return;
		}
	
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

	/* 
		PARTICLE INJECTION
		use injection position from SimData (through DetectorList)
	*/
	// get detector position as reference for injection position
	SimData &simData = fEvent.GetSimData();
	const double injHeight = simData.GetInjectionHeight();
	const double injRadius = simData.GetInjectionRadius();
	if (!(injHeight || injRadius)) 
		cout << "[WARNING] G4NeutronPrimaryGeneratorAction::GeneratePrimaries: Using default values for injection area!" << endl;

	cout << "[INFO] G4NeutronPrimaryGeneratorAction::GeneratePrimaries: Injecting particles over circumference of Radius = " << injRadius / CLHEP::m << " at Height = " << injHeight / CLHEP::m << endl;

	double circArea = CLHEP::pi * pow(injRadius, 2);
	cout << "[INFO] G4NeutronPrimaryGeneratorAction::GeneratePrimaries: Injection area A = " << circArea / (pow(CLHEP::m, 2)) << " m2, at z = " << injHeight / CLHEP::m << " m." << endl;
	
	// x and y coordinates randomly uniformed over a circumference of radius injRadius.
	double rand1 = RandFlat::shoot(0., 1.);
	double rand2 = RandFlat::shoot(0., CLHEP::twopi);
	double r = injRadius * sqrt(rand1);
	// compute injection position coordinates
	double x0 = r*cos(rand2);
	G4double y0 = r*sin(rand2);
	G4double z0 = injHeight;

	cout << "[INFO] G4NeutronPrimaryGeneratorAction::GeneratePrimaries: Injecting particle " << particleDef->GetParticleName() << " with Corsika ID: " << currParticleID << endl;
	cout << "[INFO] G4NeutronPrimaryGeneratorAction::GeneratePrimaries: Particle position = (" << x0 / CLHEP::m << ", " << y0 / CLHEP::m << ", " << z0 / CLHEP::m << ") m " << endl;
	cout << "[INFO] G4NeutronPrimaryGeneratorAction::GeneratePrimaries: Particle energy = " << fKineticEnergy / MeV << " MeV" << endl;
	
	const std::vector<double> injectionPosition = {x0, y0, z0};
	currParticle.SetInjectionPosition(injectionPosition);
	
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(fPx, fPy, -1*fPz));
	fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
	fParticleGun->SetParticleEnergy(fKineticEnergy);

	fParticleGun->GeneratePrimaryVertex(event);
}
