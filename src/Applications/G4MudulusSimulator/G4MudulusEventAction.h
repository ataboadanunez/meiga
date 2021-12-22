// definition of G4MudulusEventAction class
#ifndef G4MudulusEventAction_h 
#define G4MudulusEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "G4MudulusRunAction.h"
#include "G4MudulusPrimaryGeneratorAction.h"

#include <string>
#include <vector>


class G4MudulusEventAction : public G4UserEventAction
{
  public:
    G4MudulusEventAction();
    virtual ~G4MudulusEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);

};

#endif

