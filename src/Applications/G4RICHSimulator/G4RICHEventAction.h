// definition of G4RICHEventAction class
#ifndef G4RICHEventAction_h 
#define G4RICHEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "G4RICHRunAction.h"
#include "G4RICHPrimaryGeneratorAction.h"
#include "Event.h"

#include <string>
#include <vector>


class G4RICHEventAction : public G4UserEventAction
{
  public:
    G4RICHEventAction(Event& theEvent);
    virtual ~G4RICHEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);

  private:
    Event& fEvent;

};

#endif

