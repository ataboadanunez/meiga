// definition of G4WCDEventAction class
#ifndef G4WCDEventAction_h 
#define G4WCDEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "G4WCDRunAction.h"
#include "G4WCDPrimaryGeneratorAction.h"

#include <string>
#include <vector>


class G4WCDEventAction : public G4UserEventAction
{
  public:
    G4WCDEventAction();
    virtual ~G4WCDEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);

};

#endif

