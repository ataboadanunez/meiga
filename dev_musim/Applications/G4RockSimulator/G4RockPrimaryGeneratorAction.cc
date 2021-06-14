// implementation of the G4RockPrimaryGeneratorAction class

#include "G4RockPrimaryGeneratorAction.h"
#include "CorsikaUtilities.h"

#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include <CLHEP/Random/Randomize.h>
#include <math.h>
using CLHEP::RandFlat;
using namespace std;

G4RockPrimaryGeneratorAction::G4RockPrimaryGeneratorAction(/*ParticleInjector* partInj*/)
: G4VUserPrimaryGeneratorAction(),
	fParticleGun(new G4ParticleGun(1)),
	fInputStream(0)
	/*,
	fParticleInjector(partInj)*/
{	

	G4cout << "...G4RockPrimaryGeneratorAction..." << G4endl;

	fFileName = "/home/alvaro/data/sims/sierra_grande/muons_sierra_grande.shw";
	fInputStream = new ifstream(fFileName.c_str());

  if (!fInputStream->is_open()) {
    ostringstream msg;
    G4cout << "Unable to open the requested file : " << fFileName;
    //throw exception?;
  }

  if (fTestEnabled)
    fTextFile = new ofstream(fOutputFileName.c_str());

	// write case for injection from macro, not from file of secondaries
	
	/*
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;
	G4ParticleDefinition* particle = particleTable->FindParticle(particleName="mu+"); // de momento lo dejamos asi
	fParticleGun->SetParticleDefinition(particle);
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,-1.,0.));
	fParticleGun->SetParticleEnergy(10.*GeV);
	*/
}

G4RockPrimaryGeneratorAction::~G4RockPrimaryGeneratorAction()
{
	delete fParticleGun;
	
	fInputStream->close();
	delete fInputStream;

  if (fTestEnabled) {
    delete fTextFile;
  }
}

void 
G4RockPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	
	// extract particle ID, momentum and position from file of secondaries
	G4cout << "[DEBUG] G4RockPrimaryGeneratorAction Particle Info (CORSIKA): " << G4endl;
	string line;
  unsigned int particleId;
	do {

    if (!getline(*fInputStream, line)) {
      fInputStream->close();
    }

    istringstream iss(line);	
  	G4cout << line << G4endl;
    if (!(iss >> particleId
              >> px >> py >> pz
              >> x >> y
              >> z
              >> primaryShowerId
              >> primaryParticleId
              >> primaryEnergy
              >> primaryTheta
              >> primaryPhi)){
              //>> firstInteraction)) {
      G4cout << "Malformed line." << G4endl;
    }

  } while (particleId == 201 || particleId == 301);  // for G4's sake skip Deuterium or Tritium

  // fix up the units
  particleId = Corsika::CorsikaToPDG(particleId);
  px *= GeV; py *= GeV; pz *= GeV;
  x *= cm; y *= cm;
  z *= second;
  //primaryParticleId = io::Corsika::CorsikaToPDG(primaryParticleId);
  primaryEnergy *= GeV;
  primaryTheta *= degree;
  primaryPhi *= degree;


  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle(particleId);
  
  fParticleGun->SetParticleDefinition(particle);
  
  G4double px2 = px*px;
  G4double py2 = py*py;
  G4double pz2 = pz*pz;

  G4double particleMomentum = sqrt(px2 + py2 + pz2);
  G4double particleZenith = acos(pz / particleMomentum);
  G4double particleAzimut = atan2(-py, -px);
  /*
  G4cout << "[DEBUG] G4RockPrimaryGeneratorAction Particle Info (GEANT4): " << G4endl;
  G4cout << "        Particle Name (ID) = " << particle->GetParticleName() << " (" << particleId << ") " << G4endl;
  G4cout << "        Particle momentum / MeV = (" << px / MeV << ", " << py / MeV << ", " << pz / MeV << " )" << G4endl; 
  G4cout << "        Primary Energy / MeV = " << primaryEnergy / MeV << G4endl;
  G4cout << "        Primary Zenith / deg = " << primaryTheta / degree << G4endl;
  G4cout << "        Particle Zenith / deg = " << particleZenith / degree << G4endl;
  G4cout << "        Particle Azimut / deg = " << particleAzimut / degree << G4endl;
  */

  if (fTestEnabled)
    *fTextFile << particleId << " " << particleMomentum << " " << particleZenith / degree << " " << particleAzimut / degree << endl;

  // 
  /* inject particles following distributions according their incoming direction
    for now, we inject in a circle over the scintillator
    randomized on top of the scintillator
    x = [-Barwidth/2, BarWidth/2]
    z = [-BarLength/2, BarLength/2]
    y = fixed height
  */
  G4double delta = 5*mm;
  G4double BarWidth = 41*mm;
  //G4double BarLength = 2*BarWidth;

  const G4double injRadius = BarWidth+delta;
  const G4double injHeight = 2*cm;    
  const G4double rand = RandFlat::shoot(0., 1.);
  const G4double r = injRadius*sqrt(rand);
  const G4double phi = RandFlat::shoot(0., M_2_PI);

  G4double x0 = r * cos(phi);
  G4double y0 = injHeight;
  G4double z0 = r * sin(phi);
	
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(px, -pz, py));
	fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
  fParticleGun->SetParticleEnergy(10.*GeV);
	fParticleGun->GeneratePrimaryVertex(event);
}