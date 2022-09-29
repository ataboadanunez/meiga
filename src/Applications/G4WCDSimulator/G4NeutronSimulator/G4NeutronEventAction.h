// definition of G4NeutronEventAction class
#ifndef G4NeutronEventAction_h 
#define G4NeutronEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "G4NeutronRunAction.h"
#include "G4NeutronPrimaryGeneratorAction.h"

#include <string>
#include <vector>


class G4NeutronEventAction : public G4UserEventAction
{
  public:
    G4NeutronEventAction();
    virtual ~G4NeutronEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);

};

#endif

