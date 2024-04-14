#include "G4LeadTrackingAction.h"
#include "G4LeadSimulator.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4TrackStatus.hh>
#include <G4OpticalPhoton.hh>

#include "SimData.h"
#include "Particle.h"

using namespace std;

G4LeadTrackingAction::G4LeadTrackingAction(Event &theEvent) :
	fEvent(theEvent) 
{

}

void
G4LeadTrackingAction::PreUserTrackingAction(const G4Track* track)
{ 

}


void
G4LeadTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  
}