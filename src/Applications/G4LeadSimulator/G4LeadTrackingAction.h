#ifndef G4LeadTrackingAction_h
#define G4LeadTrackingAction_h 1

#include "Event.h"

#include <G4UserTrackingAction.hh>

class G4Track;

class G4LeadTrackingAction : public G4UserTrackingAction {
	public:
		G4LeadTrackingAction(Event& theEvent);
		virtual ~G4LeadTrackingAction() { }

		virtual void PreUserTrackingAction(const G4Track*  track) override;
    virtual void PostUserTrackingAction(const G4Track* track) override;


  private:

  	Event& fEvent;

};

#endif

