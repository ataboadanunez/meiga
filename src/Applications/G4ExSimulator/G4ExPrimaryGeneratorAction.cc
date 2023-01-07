// implementation of the G4ExPrimaryGeneratorAction class
#include "CorsikaUtilities.h"
#include "Geometry.h"

#include "G4ExSimulator.h"
#include "G4ExPrimaryGeneratorAction.h"
#include "G4ExDetectorConstruction.h"

#include "EcoMug.h"

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

G4ExPrimaryGeneratorAction::G4ExPrimaryGeneratorAction(Event& theEvent) : 
	G4VUserPrimaryGeneratorAction(),
	fParticleGun(new G4ParticleGun(1)), 
	mu_plus(0), mu_minus(0),
	fEvent(theEvent)
	
{  
	
	SimData &simData = fEvent.GetSimData();
	fUseEcoMug = (simData.GetInputMode() == simData.InputMode::eUseEcoMug);
	if (fUseEcoMug) {

		// initialize class
		fMuonGen.SetUseHSphere();
		// use coordinate system related to detector position
		fMuonGen.SetHSphereRadius(1 * CLHEP::mm);
		fMuonGen.SetHSphereCenterPosition({{0., 0., 0.}});
		mu_minus = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
		mu_plus = G4ParticleTable::GetParticleTable()->FindParticle("mu+");
	 
	 }

	fDetectorConstructor = static_cast<const G4ExDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());


}

G4ExPrimaryGeneratorAction::~G4ExPrimaryGeneratorAction()
{
	delete fParticleGun;
}


void 
G4ExPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	


	// Example using EcoMug as primary generator
	if (fUseEcoMug) {

		cout << "[INFO] G4ExPrimaryGeneratorAction::GeneratePrimaries: using EcoMug as muon generator!" << endl;
		fMuonGen.Generate();
		array<double, 3> muon_pos = fMuonGen.GetGenerationPosition();
		double x0 = muon_pos[0] * CLHEP::mm;
		double y0 = muon_pos[1] * CLHEP::mm;
		double z0 = muon_pos[2] * CLHEP::mm;
		cout << " Muon position " << x0 / CLHEP::m << " " << y0 / CLHEP::m << " " << z0 / CLHEP::m << endl;
		cout << " muon charge " << fMuonGen.GetCharge() << endl;

		double muon_ptot = fMuonGen.GetGenerationMomentum();
		double muon_theta = fMuonGen.GetGenerationTheta();
		double muon_phi   = fMuonGen.GetGenerationPhi();

		cout << " muon momentum " << muon_ptot / CLHEP::GeV << endl;

		double fPx = muon_ptot * sin(muon_theta) * cos(muon_phi) * CLHEP::GeV;
		double fPy = muon_ptot * sin(muon_theta) * sin(muon_phi) * CLHEP::GeV;
		double fPz = muon_ptot * cos(muon_theta) * CLHEP::GeV;

		if (fMuonGen.GetCharge() < 0) {
			
			fParticleGun->SetParticleDefinition(mu_minus);
			fMuonCharge = -1;
			
			} else {
				
				fParticleGun->SetParticleDefinition(mu_plus);
				fMuonCharge = 1;
		}

		fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
		fParticleGun->SetParticleMomentum(G4ParticleMomentum(fPx, fPy, fPz));

		fParticleGun->GeneratePrimaryVertex(event);


	} else{

		// particle information already loaded from input particle file
		Particle& currParticle = G4ExSimulator::currentParticle;
		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

		unsigned int currParticleID = currParticle.GetParticleId();
		int fParticleId = Corsika::CorsikaToPDG(currParticleID);
		G4ParticleDefinition* particleDef = particleTable->FindParticle(fParticleId);

		if (!particleDef) {
				cout << "[WARNING] G4ExPrimaryGeneratorAction::GeneratePrimaries: Undefined particle with Corsika ID: " << currParticleID << endl;
				return;
		}
	
		double maxHeight = fEvent.GetMaximumHeight();

		double fMomentum = currParticle.GetMomentum();
		double fKineticEnergy = currParticle.GetKineticEnergy();
		auto& particlePosition = currParticle.GetParticlePosition();

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
		const G4double injHeight = maxHeight;  
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

}
