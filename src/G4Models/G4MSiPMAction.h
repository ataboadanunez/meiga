#ifndef G4MSiPMAction_h 
#define G4MSiPMAction_h 1

#include "Event.h"
#include <G4VSensitiveDetector.hh>


class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class G4MSiPMAction : public G4VSensitiveDetector {
  public:
    G4MSiPMAction(const G4String& name, const G4int mId, const G4int sId, Event& theEvent);
    virtual ~G4MSiPMAction() { }

    virtual void Initialize(G4HCofThisEvent* const /*hce*/) override;
    virtual G4bool ProcessHits(G4Step* const step, G4TouchableHistory* const rOhist) override;
    virtual void EndOfEvent(G4HCofThisEvent* const /*hce*/) override;

  private:
  	
    Event& fEvent;

    int fModuleId = 0;
    int fSiPMId   = 0;

    std::vector<double>* fPETime;
    std::vector<std::vector<double>*>* fPETimeDistribution;
};


#endif