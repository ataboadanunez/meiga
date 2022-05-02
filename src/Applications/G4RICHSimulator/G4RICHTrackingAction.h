#ifndef G4RICHTrackingAction_h
#define G4RICHTrackingAction_h 1

#include <G4UserTrackingAction.hh>

class G4Track;

class G4RICHTrackingAction : public G4UserTrackingAction {
	public:
		G4RICHTrackingAction() { }
		~G4RICHTrackingAction() { }

		virtual void PreUserTrackingAction(const G4Track*  track) override;
    virtual void PostUserTrackingAction(const G4Track* track) override;

    bool GetFirstStepInVolume() const { return firstStepInVolume; }
    void SetFirstStepInVolume(const bool b) { firstStepInVolume = b; }


  private:

  	bool firstStepInVolume = true;

};

#endif

