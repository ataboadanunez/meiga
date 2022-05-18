// definition of G4MuDecEventAction class
#ifndef G4MuDecEventAction_h 
#define G4MuDecEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "G4MuDecRunAction.h"
#include "G4MuDecPrimaryGeneratorAction.h"
#include "Event.h"

#include <string>
#include <vector>


class G4MuDecEventAction : public G4UserEventAction
{
  public:
    G4MuDecEventAction(Event& theEvent);
    virtual ~G4MuDecEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);

    void AddCerenkov() { ++fNumCerenkov; }
    int GetNumberCerenkov() { return fNumCerenkov; }

    void AddCerenkovDelta() { ++fNumCerenkovDelta; }
    int GetNumberCerenkovDelta() { return fNumCerenkovDelta; }

    void AddStepLength(double len) { fStepLength += len; }
    double GetTrackLength() { return fStepLength; }
  private:
    Event& fEvent;
    // counter of Cerenkov photons produced by primary particle
    int fNumCerenkov = 0;
    // counter of Cerenkov photons produced by secondaries inside water (e.g. delta rays)
    int fNumCerenkovDelta = 0;
    double fStepLength = 0;
    double trackLength;
};

#endif

