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
  fEvent(theEvent)
{  

	fDetectorConstructor = static_cast<const G4CasposoDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  

}

G4CasposoPrimaryGeneratorAction::~G4CasposoPrimaryGeneratorAction()
{
  delete fParticleGun;
}


void 
G4CasposoPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  
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
  double groundThickness = simData.GetGroundThickness();
  double injHeight = 3*CLHEP::m;
  double injWidth  = 0.5 * CLHEP::m;

  const double x0 = RandFlat::shoot(-injWidth, injWidth);
  const double y0 = RandFlat::shoot(-injWidth, injWidth);
  const double z0 = injHeight;
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
  cout << "[DEBUG] InjectionPosition = (" << x0 / CLHEP::m << ", " << y0 / CLHEP::m << ", " << z0 / CLHEP::m << ") m" << endl;
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(fPx, fPy, -1*fPz));
  fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
  fParticleGun->SetParticleEnergy(fKineticEnergy);

  fParticleGun->GeneratePrimaryVertex(event);
}
