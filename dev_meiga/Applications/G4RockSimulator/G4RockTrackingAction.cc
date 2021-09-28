#include "G4RockTrackingAction.h"
#include "G4RockSimulator.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4TrackStatus.hh>

#include "Particle.h"


void
G4RockTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  firstStepInVolume = true;
  	
}


void
G4RockTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  firstStepInVolume = false;
  Particle& currParticle = G4RockSimulator::currentParticle;
  int particleId = currParticle.GetParticleId();
  double particleEnergy = currParticle.GetTotalEnergy();
  double particleZenith = currParticle.GetZenith();
  // track primaries
  if (track->GetParentID() == 0) {
  	G4cout << "TrackStoppingInfo " << particleId << " " << particleEnergy / CLHEP::MeV << " " << particleZenith / CLHEP::deg << " " << track->GetPosition()[2] / CLHEP::m << " " << track->GetVolume()->GetName() << G4endl; 
  }
}