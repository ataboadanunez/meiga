#ifndef G4RockSiPMAction_h 
#define G4RockSiPMAction_h 1

#include "Event.h"
#include "G4RockSimulator.h"
#include <G4VSensitiveDetector.hh>


class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class G4RockSiPMAction : public G4VSensitiveDetector {
  public:
    G4RockSiPMAction(const G4String& name, Event& theEvent);
    virtual ~G4RockSiPMAction() { }

    virtual void Initialize(G4HCofThisEvent* const /*hce*/) override { }
    virtual G4bool ProcessHits(G4Step* const step, G4TouchableHistory* const rOhist) override;
    virtual void EndOfEvent(G4HCofThisEvent* const /*hce*/) override { }

  private:
    Event& fEvent;
};


#endif