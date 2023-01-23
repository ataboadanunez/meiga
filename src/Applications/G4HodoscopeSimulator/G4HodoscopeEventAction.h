// definition of G4HodoscopeEventAction class
#ifndef G4HodoscopeEventAction_h 
#define G4HodoscopeEventAction_h 1

// Geant4 headers
#include "G4UserEventAction.hh"
#include "G4Event.hh"
// Meiga headers
#include "Event.h"
#include "G4HodoscopeRunAction.h"
#include "G4HodoscopePrimaryGeneratorAction.h"
#include "G4HodoscopeSimulator.h"
// C++ headers
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

class G4HodoscopeEventAction : public G4UserEventAction
{
  public:
    G4HodoscopeEventAction();
    virtual ~G4HodoscopeEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);
  
  private:
   
  friend class G4HodoscopeSimulator;

};

#endif

