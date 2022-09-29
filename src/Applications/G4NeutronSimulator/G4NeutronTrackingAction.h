#ifndef G4NeutronTrackingAction_h
#define G4NeutronTrackingAction_h 1

#include <G4UserTrackingAction.hh>

class G4Track;

class G4NeutronTrackingAction : public G4UserTrackingAction {
	public:
		G4NeutronTrackingAction() { }
		~G4NeutronTrackingAction() { }

		virtual void PreUserTrackingAction(const G4Track*  track) override;
    virtual void PostUserTrackingAction(const G4Track* track) override;

    bool GetFirstStepInVolume() const { return firstStepInVolume; }
    void SetFirstStepInVolume(const bool b) { firstStepInVolume = b; }


  private:

  	bool firstStepInVolume = true;

};

#endif

