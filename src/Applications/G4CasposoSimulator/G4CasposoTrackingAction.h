#ifndef G4CasposoTrackingAction_h
#define G4CasposoTrackingAction_h 1

#include <G4UserTrackingAction.hh>

class G4Track;

class G4CasposoTrackingAction : public G4UserTrackingAction {
	public:
		G4CasposoTrackingAction() { }
		~G4CasposoTrackingAction() { }

		virtual void PreUserTrackingAction(const G4Track*  track) override;
    virtual void PostUserTrackingAction(const G4Track* track) override;

    bool GetFirstStepInVolume() const { return firstStepInVolume; }
    void SetFirstStepInVolume(const bool b) { firstStepInVolume = b; }


  private:

  	bool firstStepInVolume = true;

};

#endif

