#include "G4BarTrackingAction.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4TrackStatus.hh>


void
G4BarTrackingAction::PreUserTrackingAction(const G4Track* const /*track*/)
{
  firstStepInVolume = true;
  
}


void
G4BarTrackingAction::PostUserTrackingAction(const G4Track* const /*track*/)
{
  firstStepInVolume = false;
}