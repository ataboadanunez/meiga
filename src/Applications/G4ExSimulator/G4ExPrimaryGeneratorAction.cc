// implementation of the G4ExPrimaryGeneratorAction class
#include "CorsikaUtilities.h"
#include "Geometry.h"

#include "G4ExSimulator.h"
#include "G4ExPrimaryGeneratorAction.h"
#include "G4ExDetectorConstruction.h"

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
	
	// example using EcoMug as particle generator. 
	// this example should be part of src/G4Models/G4MGeneratePrimaries.{cc,h}
	SimData &simData = fEvent.GetSimData();
	injMode = simData.GetInjectionMode();

	fUseEcoMug = (simData.GetInputMode() == simData.InputMode::eUseEcoMug);

	if (fUseEcoMug || (injMode == SimData::InjectionMode::eHalfSphere)) {
		// setting generation properties	
		cout << "[INFO] G4ExPrimaryGeneratorAction::GeneratePrimaries: Primary generation using EcoMug" << endl;
		cout << "Particles injected in " << simData.GetInjectionModeName() << endl; 
		// initialize class
		fMuonGen.SetUseHSphere();
		// use coordinate system related to detector position
		double injectionRadius = simData.GetInjectionRadius();
		vector<double> injectionOrigin = simData.GetInjectionOrigin();
		double oX = injectionOrigin[0];
		double oY = injectionOrigin[1];
		double oZ = injectionOrigin[2];

		fMuonGen.SetHSphereRadius(injectionRadius);
		cout << "[INFO] G4ExPrimaryGeneratorAction::GeneratePrimaries: Setting InjectionRadius = " << injectionRadius / CLHEP::m << endl;
		fMuonGen.SetHSphereCenterPosition({{oX, oY, oZ}});
		cout << "[INFO] G4ExPrimaryGeneratorAction::GeneratePrimaries: Setting InjectionOrigin = (" << oX / CLHEP::m << ", " << oY / CLHEP::m << ", " << oZ / CLHEP::m << ") m" << endl;
	 
	 }

	// case of only muon generation (e.g. EcoMug)
	mu_minus = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
	mu_plus  = G4ParticleTable::GetParticleTable()->FindParticle("mu+");
	
	fDetectorConstructor = static_cast<const G4ExDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());


}

G4ExPrimaryGeneratorAction::~G4ExPrimaryGeneratorAction()
{
	delete fParticleGun;
}


void 
G4ExPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	


	/* example using two injection modes: eCircle and eHalfSphere
	- eCircle generates particles in a circumference of given radius at a given height
	- eHalfSphere uses EcoMug library for generating particles in a half-sphere of given radius
	*/

	SimData &simData = fEvent.GetSimData();
	injMode = simData.GetInjectionMode();

	double x0;
	double y0;
	double z0;

	// Example using EcoMug as primary generator
	if (fUseEcoMug) {

		cout << "[INFO] G4ExPrimaryGeneratorAction::GeneratePrimaries: using EcoMug as muon generator!" << endl;
		fMuonGen.Generate();
		array<double, 3> muon_pos = fMuonGen.GetGenerationPosition();
		x0 = muon_pos[0] * CLHEP::mm;
		y0 = muon_pos[1] * CLHEP::mm;
		z0 = muon_pos[2] * CLHEP::mm;
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

		cout << "[DEBUG] TestInjection " << fMuonCharge << " " << x0 / CLHEP::m << " " << y0 / CLHEP::m << " " << z0 / CLHEP::m << " " << fPx / CLHEP::GeV << " " << fPy / CLHEP::GeV << " " << fPz / CLHEP::GeV << " " << muon_ptot << " " << muon_theta * 180 / M_PI << " " << muon_phi * 180 / M_PI << endl;

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

		injMode = simData.GetInjectionMode();
		// switch between injection modes
		if (injMode == SimData::InjectionMode::eCircle) {
			
			double injRadius = simData.GetInjectionRadius();
			double injHeight = simData.GetInjectionHeight();

			double rand = RandFlat::shoot(0., 1.);
			double  r = injRadius*sqrt(rand);
			double minPhi = simData.GetInjectionMinPhi() * (CLHEP::twopi / 360);
			double maxPhi = simData.GetInjectionMaxPhi() * (CLHEP::twopi / 360);
			double phi = RandFlat::shoot(minPhi, maxPhi);

			x0 = r*cos(phi);
			y0 = r*sin(phi);
			z0 = injHeight;
	
		} 

		else if (injMode == SimData::InjectionMode::eHalfSphere) {
			// use EcoMug for HalfSphere generation (only particle position)
			fMuonGen.Generate();
			array<double, 3> particlePos = fMuonGen.GetGenerationPosition();
			x0 = particlePos[0] * CLHEP::mm;
			y0 = particlePos[1] * CLHEP::mm;
			z0 = particlePos[2] * CLHEP::mm;

		}

		else if (injMode == SimData::InjectionMode::eUnknown) {
			// inject particles from origin given in XML
			vector<double> particlePos = simData.GetInjectionOrigin();
			x0 = particlePos[0];
			y0 = particlePos[1];
			z0 = particlePos[2];
		}
		else {
			x0 = 0;
			y0 = 0;
			z0 = 0;
		}
		
		const std::vector<double> injectionPosition = {x0, y0, z0};
		currParticle.SetInjectionPosition(injectionPosition);
		fParticleGun->SetParticleMomentumDirection(G4ThreeVector(fPx, fPy, -1*fPz));
		fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
		fParticleGun->SetParticleEnergy(fKineticEnergy);
		fParticleGun->GeneratePrimaryVertex(event);
		
		cout << "[DEBUG] TestInjection " << fParticleId << " " << x0 / CLHEP::m << " " << y0 / CLHEP::m << " " << z0 / CLHEP::m << " " << fPx / CLHEP::GeV << " " << fPy / CLHEP::GeV << " " << fPz / CLHEP::GeV << " " << fMomentum << " " << particleZenith * 180 / M_PI << " " << particleAzimut * 180 / M_PI << endl;

	}

}
