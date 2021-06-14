// definition of G4RockEventAction class
#ifndef G4RockEventAction_h 
#define G4RockEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "G4RockRunAction.h"
#include "G4RockPrimaryGeneratorAction.h"

#include <string>
#include <vector>


class G4RockEventAction : public G4UserEventAction
{
  public:
    G4RockEventAction(G4RockRunAction* runAction, G4RockPrimaryGeneratorAction* primaryGenAction);
    virtual ~G4RockEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);

    G4RockRunAction* fRunAction;
    G4RockPrimaryGeneratorAction* fPriGenAction;
    
    std::vector<double> GetSiPMTrace() { return fSiPMTrace; }
  private:
    std::vector<double> fSiPMTrace;
    int fTraceLength = 300; // hardcoded. why static?
};

#endif

