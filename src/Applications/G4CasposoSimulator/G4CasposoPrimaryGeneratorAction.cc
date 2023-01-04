#include "CorsikaUtilities.h"
#include "Geometry.h"

#include "G4CasposoSimulator.h"
#include "G4CasposoPrimaryGeneratorAction.h"
#include "G4CasposoDetectorConstruction.h"

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

G4CasposoPrimaryGeneratorAction::G4CasposoPrimaryGeneratorAction(Event& theEvent) : 
  G4VUserPrimaryGeneratorAction(),
  fParticleGun(new G4ParticleGun(1)),
  mu_plus(0), mu_minus(0),
  fEvent(theEvent)
{  

  if (fUseEcoMug) {

    fMuonGen.SetUseHSphere();
    // use coordinate system related to detector position
    fMuonGen.SetHSphereRadius(220 * CLHEP::m);
    fMuonGen.SetHSphereCenterPosition({{0., 0., 0.}});
    // limits for momentum and generation position
    fMuonGen.SetMinimumMomentum(100);
    fMuonGen.SetMaximumMomentum(1e4); 
    fMuonGen.SetHSphereMinPositionTheta(60 * (M_PI / 180));
    fMuonGen.SetHSphereMaxPositionTheta(90 * (M_PI / 180));
    fMuonGen.SetHSphereMinPositionPhi(75 * (M_PI / 180));
    fMuonGen.SetHSphereMaxPositionPhi(105 * (M_PI / 180));

    // inject mu_minus or mu_plus according to ratio of atmospheric muons
    mu_minus = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
    mu_plus = G4ParticleTable::GetParticleTable()->FindParticle("mu+");

  }

	fDetectorConstructor = static_cast<const G4CasposoDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  

}

G4CasposoPrimaryGeneratorAction::~G4CasposoPrimaryGeneratorAction()
{
  delete fParticleGun;
}


void 
G4CasposoPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
   

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
    double x0 = muon_pos[0] * CLHEP::mm;
    double y0 = muon_pos[1] * CLHEP::mm;
    double z0 = muon_pos[2] * CLHEP::mm;


    double muon_ptot = fMuonGen.GetGenerationMomentum();
    double muon_theta = fMuonGen.GetGenerationTheta();
    double muon_phi   = fMuonGen.GetGenerationPhi();

    // DEBUG
    x0 = 0 * CLHEP::m;
    y0 = 0 * CLHEP::m;
    z0 = 10 * CLHEP::m;
    muon_theta = 0;

    double fPx = muon_ptot * sin(muon_theta) * cos(muon_phi) * CLHEP::GeV;
    double fPy = muon_ptot * sin(muon_theta) * sin(muon_phi) * CLHEP::GeV;
    double fPz = -1*muon_ptot * cos(muon_theta) * CLHEP::GeV;



    fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
    fParticleGun->SetParticleMomentum(G4ParticleMomentum(fPx, fPy, fPz));

    if (fMuonGen.GetCharge() < 0) {
      fParticleGun->SetParticleDefinition(mu_minus);
      fMuonCharge = -1;
    } else {
      fParticleGun->SetParticleDefinition(mu_plus);
      fMuonCharge = 1;
    }

    fParticleGun->GeneratePrimaryVertex(event);
    cout << "[DEBUG] MinPhi = " << fMuonGen.GetMinimumPhi() * 180 / M_PI << " MaxPhi = " << fMuonGen.GetMaximumPhi() * 180 / M_PI << endl;
    cout << "[DEBUG] Phi = " << muon_phi * 180 / M_PI << " Theta = " << muon_theta * 180 / M_PI << endl;
    // charge | x | y | z | px | py | pz | pTot
    cout << "[DEBUG] TestInjection " << fMuonCharge << " " << x0 / CLHEP::m << " " << y0 / CLHEP::m << " " << z0 / CLHEP::m << " " << fPx / CLHEP::GeV << " " << fPy / CLHEP::GeV << " " << fPz / CLHEP::GeV << " " << muon_ptot << " " << muon_theta * 180 / M_PI << " " << muon_phi * 180 / M_PI << endl;

  }

  else {

    // get particle information
    Particle& currParticle = G4CasposoSimulator::currentParticle;
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    unsigned int currParticleID = currParticle.GetParticleId();
    int fParticleId = Corsika::CorsikaToPDG(currParticleID);
    G4ParticleDefinition* particleDef = particleTable->FindParticle(fParticleId);

    if (!particleDef) {
        cout << "[WARNING] G4CasposoPrimaryGeneratorAction::GeneratePrimaries: Undefined particle with Corsika ID: " << currParticleID << endl;
        return;
      }
    
    // 
    auto& simData = fEvent.GetSimData();
    
    // particle injection on ground surface
    // get ground thickness (depth, along z-axis)
    // double groundThickness = simData.GetGroundThickness();
    double injHeight = 1.2*CLHEP::m;
    double injWidth  = 0.25 * CLHEP::m;
    // HARDCODED
    const double x0 = 0 * CLHEP::m; //RandFlat::shoot(-injWidth, injWidth);
    const double y0 = 195 * CLHEP::m; //RandFlat::shoot(-injWidth, injWidth);
    const double z0 = 70 * CLHEP::m;
    // get particle energy and momentum direction
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

    const std::vector<double> injectionPosition = {x0, y0, z0};
    currParticle.SetInjectionPosition(injectionPosition);
    cout << "[INFO] InjectionPosition = (" << x0 / CLHEP::m << ", " << y0 / CLHEP::m << ", " << z0 / CLHEP::m << ") m" << endl;
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(fPx, fPy, -1*fPz));
    fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
    fParticleGun->SetParticleEnergy(fKineticEnergy);

    fParticleGun->GeneratePrimaryVertex(event);

  }

  
}
