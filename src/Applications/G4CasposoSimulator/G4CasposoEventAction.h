// definition of G4CasposoEventAction class
#ifndef G4CasposoEventAction_h 
#define G4CasposoEventAction_h 1

// Geant4 headers
#include "G4UserEventAction.hh"
#include "G4Event.hh"
// Meiga headers
#include "Event.h"
#include "G4CasposoRunAction.h"
#include "G4CasposoPrimaryGeneratorAction.h"
#include "G4CasposoSimulator.h"
// C++ headers
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

class G4CasposoEventAction : public G4UserEventAction
{
  public:
    G4CasposoEventAction();
    virtual ~G4CasposoEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);
  
  private:
   
  friend class G4CasposoSimulator;

};

#endif

