// definition of G4AndesEventAction class
#ifndef G4AndesEventAction_h 
#define G4AndesEventAction_h 1

// Geant4 headers
#include "G4UserEventAction.hh"
#include "G4Event.hh"
// Meiga headers
#include "Event.h"
#include "G4AndesRunAction.h"
#include "G4AndesPrimaryGeneratorAction.h"
#include "G4AndesSimulator.h"
// C++ headers
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

class G4AndesEventAction : public G4UserEventAction
{
  public:
    G4AndesEventAction();
    virtual ~G4AndesEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);
  
  private:
   
  friend class G4AndesSimulator;

};

#endif

