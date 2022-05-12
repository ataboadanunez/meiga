#ifndef G4MuDecTrackingAction_h
#define G4MuDecTrackingAction_h 1

#include <G4UserTrackingAction.hh>
#include "G4MuDecEventAction.h"

class G4Track;

class G4MuDecTrackingAction : public G4UserTrackingAction {
	public:
		G4MuDecTrackingAction(G4MuDecEventAction* G4Event, const bool countCerenkov);
		virtual ~G4MuDecTrackingAction() { }

		virtual void PreUserTrackingAction(const G4Track*  track) override;
    virtual void PostUserTrackingAction(const G4Track* track) override;

    bool GetFirstStepInVolume() const { return firstStepInVolume; }
    void SetFirstStepInVolume(const bool b) { firstStepInVolume = b; }


  private:

  	G4MuDecEventAction *fEventAction;
  	bool firstStepInVolume = true;
  	bool fCountCerenkov;


};

#endif

