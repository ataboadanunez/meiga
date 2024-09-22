// implementation of the G4LeadRunAction class
#include "G4Timer.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LeadRunAction.h"

using namespace std;

G4LeadRunAction::G4LeadRunAction(Event& theEvent)
 : G4UserRunAction(),
 fEvent(theEvent)
{	
}

G4LeadRunAction::~G4LeadRunAction()
{	
}

void 
G4LeadRunAction::BeginOfRunAction(const G4Run*)
{
}

void 
G4LeadRunAction::EndOfRunAction(const G4Run*)
{ 
}
