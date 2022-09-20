#ifndef G4MudulusTrackingAction_h
#define G4MudulusTrackingAction_h 1

#include <G4UserTrackingAction.hh>

class G4Track;

class G4MudulusTrackingAction : public G4UserTrackingAction {
	public:
		G4MudulusTrackingAction() { }
		~G4MudulusTrackingAction() { }

		virtual void PreUserTrackingAction(const G4Track*  track) override;
    virtual void PostUserTrackingAction(const G4Track* track) override;

    bool GetFirstStepInVolume() const { return firstStepInVolume; }
    void SetFirstStepInVolume(const bool b) { firstStepInVolume = b; }


  private:

  	bool firstStepInVolume = true;

};

#endif

