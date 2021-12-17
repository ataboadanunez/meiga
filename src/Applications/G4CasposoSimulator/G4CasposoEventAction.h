// definition of G4CasposoEventAction class
#ifndef G4CasposoEventAction_h 
#define G4CasposoEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "G4CasposoRunAction.h"
#include "G4CasposoPrimaryGeneratorAction.h"

#include <string>
#include <vector>


class G4CasposoEventAction : public G4UserEventAction
{
  public:
    G4CasposoEventAction();
    virtual ~G4CasposoEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);

};

#endif

