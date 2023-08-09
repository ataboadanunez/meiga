#include "G4HodoscopeEventAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "OptDeviceSimData.h"

using namespace std;

G4HodoscopeEventAction::G4HodoscopeEventAction(Event& theEvent) : 
	fEvent(theEvent),
	G4UserEventAction()
{
		
}

G4HodoscopeEventAction::~G4HodoscopeEventAction()
{
		
}

void
G4HodoscopeEventAction::BeginOfEventAction(const G4Event*)
{

}

void 
G4HodoscopeEventAction::EndOfEventAction(const G4Event*)
{

}

