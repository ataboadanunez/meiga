#ifndef G4MuDecTrackingAction_h
#define G4MuDecTrackingAction_h 1

#include "Event.h"
#include "G4MuDecEventAction.h"

#include <G4UserTrackingAction.hh>


class G4Track;

class G4MuDecTrackingAction : public G4UserTrackingAction {
	public:
		G4MuDecTrackingAction(G4MuDecEventAction* G4Event, Event& theEvent, const bool countCerenkov);
		virtual ~G4MuDecTrackingAction() { }

		virtual void PreUserTrackingAction(const G4Track*  track) override;
		virtual void PostUserTrackingAction(const G4Track* track) override;


	private:

		G4MuDecEventAction* fEventAction;
		Event& fEvent;
		bool fCountCerenkov;
		


};

#endif

