#include "CorsikaUtilities.h"
#include "Geometry.h"

#include "G4HodoscopeSimulator.h"
#include "G4HodoscopePrimaryGeneratorAction.h"
#include "G4HodoscopeDetectorConstruction.h"

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

G4HodoscopePrimaryGeneratorAction::G4HodoscopePrimaryGeneratorAction(Event& theEvent) : 
  G4VUserPrimaryGeneratorAction(),
  fParticleGun(new G4ParticleGun(1)),
  mu_plus(0), mu_minus(0),
  fEvent(theEvent)
{  

  // lines below are a copy of G4ExPrimaryGeneratorAction.cc
  // should be part of an external function

  SimData &simData = fEvent.GetSimData();
  injMode = simData.GetInjectionMode();
  fUseEcoMug = (simData.GetInputMode() == simData.InputMode::eUseEcoMug);

  if (fUseEcoMug || (injMode == SimData::InjectionMode::eHalfSphere)) {

    cout << "[INFO] G4HodoscopePrimaryGeneratorAction::GeneratePrimaries: Primary generation using EcoMug" << endl;
    cout << "Particles injected in " << simData.GetInjectionModeName() << endl;

    fMuonGen.SetUseHSphere();
    // use coordinate system related to detector position
    double injectionRadius = simData.GetInjectionRadius();
    vector<double> injectionOrigin = simData.GetInjectionOrigin();
    double oX = injectionOrigin[0];
    double oY = injectionOrigin[1];
    double oZ = injectionOrigin[2];

    fMuonGen.SetHSphereRadius(injectionRadius);
    cout << "[INFO] G4HodoscopePrimaryGeneratorAction::GeneratePrimaries: Setting InjectionRadius = " << injectionRadius / CLHEP::m << endl;
    fMuonGen.SetHSphereCenterPosition({{oX, oY, oZ}});
    // limits for momentum and generation position
    fMuonGen.SetMinimumMomentum(1e-1);
    fMuonGen.SetMaximumMomentum(1e4);
    double minPhi = simData.GetInjectionMinPhi();
    double maxPhi = simData.GetInjectionMaxPhi();
    double minTheta = simData.GetInjectionMinTheta();
    double maxTheta = simData.GetInjectionMaxTheta();

    fMuonGen.SetHSphereMinPositionTheta(minTheta * (M_PI / 180));
    fMuonGen.SetHSphereMaxPositionTheta(maxTheta * (M_PI / 180));
    fMuonGen.SetHSphereMinPositionPhi(minPhi * (M_PI / 180));
    fMuonGen.SetHSphereMaxPositionPhi(maxPhi * (M_PI / 180));

    // inject mu_minus or mu_plus according to ratio of atmospheric muons
    mu_minus = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
    mu_plus = G4ParticleTable::GetParticleTable()->FindParticle("mu+");

  }

	fDetectorConstructor = static_cast<const G4HodoscopeDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  

}

G4HodoscopePrimaryGeneratorAction::~G4HodoscopePrimaryGeneratorAction()
{
  delete fParticleGun;
}


void 
G4HodoscopePrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  
  SimData &simData = fEvent.GetSimData();
  injMode = simData.GetInjectionMode();

  double x0;
  double y0;
  double z0;

  if (fUseEcoMug) {
    
    // big fat comment here...
    // use EcoMug for half-sphere muon injection
    // consider the atmospheric muons charge ratio for 
    // muon generation
    // this ratio is R = N_mu+ / N_mu- = 1.2766
    // almost constant at energies below 100 GeV
    // https://arxiv.org/pdf/1005.5332.pdf
  
    fMuonGen.Generate();
    array<double, 3> muon_pos = fMuonGen.GetGenerationPosition();
    x0 = muon_pos[0] * CLHEP::mm;
    y0 = muon_pos[1] * CLHEP::mm;
    z0 = muon_pos[2] * CLHEP::mm;


    double muon_ptot = fMuonGen.GetGenerationMomentum();
    double muon_theta = fMuonGen.GetGenerationTheta();
    double muon_phi   = fMuonGen.GetGenerationPhi();

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
    cout << "[DEBUG] MinPhi = " << fMuonGen.GetMinimumPhi() * 180 / M_PI << " MaxPhi = " << fMuonGen.GetMaximumPhi() * 180 / M_PI << endl;
    cout << "[DEBUG] Phi = " << muon_phi * 180 / M_PI << " Theta = " << muon_theta * 180 / M_PI << endl;
    // charge | x | y | z | px | py | pz | pTot
    cout << "[DEBUG] TestInjection " << fMuonCharge << " " << x0 / CLHEP::m << " " << y0 / CLHEP::m << " " << z0 / CLHEP::m << " " << fPx / CLHEP::GeV << " " << fPy / CLHEP::GeV << " " << fPz / CLHEP::GeV << " " << muon_ptot << " " << muon_theta * 180 / M_PI << " " << muon_phi * 180 / M_PI << endl;

  }

  else {

    // get particle information
    Particle& currParticle = G4HodoscopeSimulator::currentParticle;
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    unsigned int currParticleID = currParticle.GetParticleId();
    int fParticleId = Corsika::CorsikaToPDG(currParticleID);
    G4ParticleDefinition* particleDef = particleTable->FindParticle(fParticleId);

    if (!particleDef) {
        cout << "[WARNING] G4HodoscopePrimaryGeneratorAction::GeneratePrimaries: Undefined particle with Corsika ID: " << currParticleID << endl;
        return;
      }
    
    
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
