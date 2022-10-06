#include "G4NeutronTrackingAction.h"
#include "G4NeutronSimulator.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4TrackStatus.hh>

#include "Particle.h"


void
G4NeutronTrackingAction::PreUserTrackingAction(const G4Track* /*track*/)
{ 
  
}


void
G4NeutronTrackingAction::PostUserTrackingAction(const G4Track* /*track*/)
{
  // firstStepInVolume = false;
  // Particle& currParticle = G4NeutronSimulator::currentParticle;
  // int particleId = currParticle.GetParticleId();
  // double particleEnergy = currParticle.GetTotalEnergy();
  // double particleZenith = currParticle.GetZenith();
  // G4ThreeVector trackPosition = track->GetPosition();
  // std::vector<double> injectPosition = currParticle.GetInjectionPosition();
  // double trackZ = trackPosition[2] / CLHEP::m;
  // double injectZ = injectPosition[2] / CLHEP::m;

  // double depth = injectZ - trackZ;

  // // track primaries
  // if (track->GetParentID() == 0) {
  // 	G4cout << "TrackStoppingInfo " << particleId << " " << particleEnergy / CLHEP::MeV << " " << particleZenith / CLHEP::deg << " " << depth << " " << track->GetVolume()->GetName() << G4endl; 
  // }
}