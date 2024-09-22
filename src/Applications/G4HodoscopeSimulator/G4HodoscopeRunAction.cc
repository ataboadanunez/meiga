// implementation of the G4HodoscopeRunAction class
#include "G4Timer.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4HodoscopeRunAction.h"

using namespace std;

G4HodoscopeRunAction::G4HodoscopeRunAction(Event& theEvent) 
	: G4UserRunAction(), fEvent(theEvent)
{	
}

G4HodoscopeRunAction::~G4HodoscopeRunAction()
{	
}

void 
G4HodoscopeRunAction::BeginOfRunAction(const G4Run*)
{
}

void 
G4HodoscopeRunAction::EndOfRunAction(const G4Run*)
{ 
}
