#include "G4RICHSimulator.h"
#include "G4RICHPrimaryGeneratorAction.h"
#include "G4RICHConstruction.h"

#include "CorsikaUtilities.h"
#include "Geometry.h"
#include "Detector.h"
#include "SimData.h"

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

G4RICHPrimaryGeneratorAction::G4RICHPrimaryGeneratorAction(Event& theEvent) : 
	G4VUserPrimaryGeneratorAction(),
	fParticleGun(new G4ParticleGun(1)),
	fEvent(theEvent)
	
{  

	fDetectorConstructor = static_cast<const G4RICHConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	

}

G4RICHPrimaryGeneratorAction::~G4RICHPrimaryGeneratorAction()
{
	delete fParticleGun;
}


void 
G4RICHPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	
	// get particle information
	Particle& currParticle = G4RICHSimulator::currentParticle;
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

	unsigned int currParticleID = currParticle.GetParticleId();
	int fParticleId = Corsika::CorsikaToPDG(currParticleID);
	G4ParticleDefinition* particleDef = particleTable->FindParticle(fParticleId);

	if (!particleDef) {
			cout << "[WARNING] G4RICHPrimaryGeneratorAction::GeneratePrimaries: Undefined particle with Corsika ID: " << currParticleID << endl;
			return;
		}
	
	fParticleGun->SetParticleDefinition(particleDef);

	// particle injection according to detector dimensions and injection mode
	auto& simData = fEvent.GetSimData();
	SimData::InjectionMode injectionMode = simData.GetInjectionMode();
	
	auto& detector = fEvent.GetDetector(0);
	double tankHeight = detector.GetTankHeight();
	double tankRadius = detector.GetTankRadius();
	double tankThickness = detector.GetTankThickness();
	double maxHeight  = fEvent.GetMaximumHeight();
	
	G4double injRadius = tankRadius + tankThickness;
	G4double injHeight;
	G4double r, phi;
	G4double x0, y0, z0;

	// get particle energy and momentum direction
	double fKineticEnergy = currParticle.GetKineticEnergy();
	const std::vector<double> particleMomentumDirection = currParticle.GetDirection();
	
	double fPx = particleMomentumDirection.at(0);
	double fPy = particleMomentumDirection.at(1);
	double fPz = particleMomentumDirection.at(2);
	
	// change direction in the Z axis in case of standard injection mode
	if (injectionMode == SimData::eRandom || injectionMode == SimData::ePositionFromFile)
		fPz = -1*fPz; 

	G4double px2 = fPx*fPx;
	G4double py2 = fPy*fPy;
	G4double pz2 = fPz*fPz;

	G4double particleMomentum = sqrt(px2 + py2 + pz2);
	G4double particleAzimut = atan2(-fPy, -fPx);
	G4double particleZenith = acos(fPz / particleMomentum);
	currParticle.SetAzimuth(particleAzimut);
	currParticle.SetZenith(particleZenith);

	// injection inside the detector volume
	if (injectionMode == SimData::eInsideDetector) {
		
		injHeight = RandFlat::shoot(0., tankHeight);
		G4double rand = RandFlat::shoot(0., 1.);
		r = injRadius*sqrt(rand);
		phi = RandFlat::shoot(0., CLHEP::twopi);
		
		x0 = r*cos(phi);
		y0 = r*sin(phi);
		z0 = injHeight;

	} 
	// injection over virtual cylinder of dimensions slightly bigger than tank
	else if (injectionMode == SimData::eRandom) {
 	
 		// dimensions of virtual cylinder
		injHeight = ( tankHeight > maxHeight ? tankHeight : maxHeight) + 10*CLHEP::cm; // slightly above detector height
		injRadius += 2*CLHEP::cm;
		G4double EffTopArea = CLHEP::pi * sqr(injRadius) * cos(particleZenith);
		G4double EffSideArea = 2 * injRadius * injHeight * sin(particleZenith);
		
		// compute probability according to particle zenith angle
		G4double probSeeTop = EffTopArea / (EffSideArea + EffTopArea);
		G4double rand1 = RandFlat::shoot(0., 1.);

		// particle injected on the top
		if (rand1 <= probSeeTop) {

			G4double rand2 = RandFlat::shoot(0., 1.); 
			r = injRadius * sqrt(rand2);
			phi = RandFlat::shoot(0., CLHEP::twopi);

			x0 = r * cos(phi);
			y0 = r * sin(phi);
			z0 = injHeight;
	
		}
		// particle injected on the side
		else {

			G4double rand3 = RandFlat::shoot(0., 1.);
			G4double rand4 = RandFlat::shoot(0., 1.);

			// correct azimuth distribution
			phi = particleAzimut + asin(1-2*rand3);
			if (phi > CLHEP::twopi)
				phi -= CLHEP::pi;
			else if (phi < 0)
				phi += CLHEP::pi;

			x0 = injRadius * cos(phi);
			y0 = injRadius * sin(phi);
			z0 = injHeight * rand4;

		}

		
	} 

	
	const std::vector<double> injectionPosition = {x0, y0, z0};
	currParticle.SetInjectionPosition(injectionPosition);
	
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(fPx, fPy, fPz));
	fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));

	fParticleGun->SetParticleEnergy(fKineticEnergy);
	fParticleGun->GeneratePrimaryVertex(event);
	
	// print injection information for do the check
	std::cout << "[DEBUG] TestInjection: " << x0 / cm << " " << y0 / cm << " " << z0 / cm << " " << fKineticEnergy / MeV << std::endl;

}
