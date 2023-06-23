// definition of the G4TandarSteppingAction class

#ifndef G4TandarSteppingAction_h
#define G4TandarSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4TandarDetectorConstruction;
class G4TandarEventAction;

/* Stepping action class.
     UserSteppingAction collects info at a step level
*/

class G4TandarSteppingAction : public G4UserSteppingAction
{
  public:
    G4TandarSteppingAction(G4TandarEventAction* event, Event& theEvent);
    virtual ~G4TandarSteppingAction();
    virtual void UserSteppingAction(const G4Step* step);

  private:
    G4TandarEventAction* fEventAction;
    Event& fEvent;
};

#endif