#include "G4RockTrackingAction.h"
#include "G4RockSimulator.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4TrackStatus.hh>

#include "Particle.h"


void
G4RockTrackingAction::PreUserTrackingAction(const G4Track* track)
{ 
  fIsMuonDecay = false;
}


void
G4RockTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  
  Particle& currParticle = G4RockSimulator::currentParticle;
  int primaryId = currParticle.GetParticleId();
  double particleEnergy = currParticle.GetTotalEnergy();
  double particleZenith = currParticle.GetZenith();
  G4ThreeVector trackPosition = track->GetPosition();
  std::vector<double> injectPosition = currParticle.GetInjectionPosition();
  double trackZ = trackPosition[2] / CLHEP::m;
  double injectZ = injectPosition[2] / CLHEP::m;

  double depth = injectZ - trackZ;
  fIsMuonDecay = GetIsMuonDecay();
  const int particleId = track->GetDefinition()->GetPDGEncoding();
  // track primaries and decay muons
  if ((track->GetParentID() == 0) ||  (particleId == Particle::eMuon) || (particleId == Particle::eAntiMuon)) {
  	G4cout << "TrackStoppingInfo " << primaryId << " " << particleId << " " << particleEnergy / CLHEP::MeV << " " << particleZenith / CLHEP::deg << " " << depth << " " << track->GetVolume()->GetName() << G4endl; 
  }
}