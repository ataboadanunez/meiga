#ifndef G4HodoscopeTrackingAction_h
#define G4HodoscopeTrackingAction_h 1

#include "Event.h"

#include <G4UserTrackingAction.hh>

class G4Track;

class G4HodoscopeTrackingAction : public G4UserTrackingAction {
	public:
		G4HodoscopeTrackingAction(Event& theEvent);
		virtual ~G4HodoscopeTrackingAction() { }

		virtual void PreUserTrackingAction(const G4Track*  track) override;
    virtual void PostUserTrackingAction(const G4Track* track) override;


  private:

  	Event& fEvent;

};

#endif

