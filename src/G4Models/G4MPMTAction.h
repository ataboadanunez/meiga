#ifndef G4MPMTAction_h 
#define G4MPMTAction_h 1

#include "Event.h"
#include <G4VSensitiveDetector.hh>


class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class G4MPMTAction : public G4VSensitiveDetector {
  public:
    G4MPMTAction(const G4String& name, const G4int dId, const G4int oId, Event& theEvent);
    virtual ~G4MPMTAction() { }

    virtual void Initialize(G4HCofThisEvent* const /*hce*/) override;
    virtual G4bool ProcessHits(G4Step* const step, G4TouchableHistory* const rOhist) override;
    virtual void EndOfEvent(G4HCofThisEvent* const /*hce*/) override;

  private:
    
    Event& fEvent;

    int fDetectorId = 0;
    int fOptDeviceId = 0;
    int NumCerenkovPhotons;
    int NumPE;
    int NumPEMuDecay;

    std::vector<double> fPETime;
    std::vector<double> fPETimeComp;
    std::vector<double> fPETimeMuDecay;
    // std::vector<std::vector<double>*>* fPETimeDistribution;
};


#endif