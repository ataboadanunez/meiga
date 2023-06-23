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
    G4TandarEventAction(Event& theEvent);
    virtual ~G4TandarEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);
    
    std::vector<int> fBarsX1;
    std::vector<int> fBarsY1;
    bool trig1;
    bool trigX1;
    bool trigY1;

    std::vector<int> fBarsX2;
    std::vector<int> fBarsY2;
    bool trig2;
    bool trigX2;
    bool trigY2;

    std::vector<int> fBarsX3;
    std::vector<int> fBarsY3;
    bool trig3;
    bool trigX3;
    bool trigY3;

    int fNBars;
    
  private:
   
   Event& fEvent;
   std::ofstream fOutFile;

  friend class G4TandarSimulator;


};

#endif


