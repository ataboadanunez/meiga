#include "G4HodoscopeTrackingAction.h"
#include "G4HodoscopeSimulator.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4TrackStatus.hh>
#include <G4OpticalPhoton.hh>

#include "SimData.h"
#include "Particle.h"

using namespace std;

G4HodoscopeTrackingAction::G4HodoscopeTrackingAction(Event &theEvent) :
	fEvent(theEvent) 
{

}

void
G4HodoscopeTrackingAction::PreUserTrackingAction(const G4Track* track)
{ 

}


void
G4HodoscopeTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  
}