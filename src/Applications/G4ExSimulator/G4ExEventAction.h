// definition of G4ExEventAction class
#ifndef G4ExEventAction_h 
#define G4ExEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "G4ExRunAction.h"
#include "G4ExPrimaryGeneratorAction.h"

#include <string>
#include <vector>


class G4ExEventAction : public G4UserEventAction
{
  public:
    G4ExEventAction();
    virtual ~G4ExEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);

};

#endif

