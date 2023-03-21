// definition of G4TandarEventAction class
#ifndef G4TandarEventAction_h 
#define G4TandarEventAction_h 1

// Geant4 headers
#include "G4UserEventAction.hh"
#include "G4Event.hh"
// Meiga headers
#include "Event.h"
#include "G4TandarRunAction.h"
#include "G4TandarSimulator.h"
// C++ headers
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

class G4TandarEventAction : public G4UserEventAction
{
  public:
    G4TandarEventAction();
    virtual ~G4TandarEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);
  
  private:
   
  friend class G4TandarSimulator;

};

#endif

