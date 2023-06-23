#ifndef G4TandarTrackingAction_h
#define G4TandarTrackingAction_h 1

#include <G4UserTrackingAction.hh>

class G4Track;

class G4TandarTrackingAction : public G4UserTrackingAction {
	public:
		G4TandarTrackingAction() { }
		~G4TandarTrackingAction() { }

		virtual void PreUserTrackingAction(const G4Track*  track) override;
    virtual void PostUserTrackingAction(const G4Track* track) override;

    bool GetFirstStepInVolume() const { return firstStepInVolume; }
    void SetFirstStepInVolume(const bool b) { firstStepInVolume = b; }


  private:

  	bool firstStepInVolume = true;

};

#endif

