#ifndef G4BarTrackingAction_h
#define G4BarTrackingAction_h 1

#include <G4UserTrackingAction.hh>

class G4Track;

class G4BarTrackingAction : public G4UserTrackingAction {
	public:
		G4BarTrackingAction() { }
		~G4BarTrackingAction() { }

		virtual void PreUserTrackingAction(const G4Track* const track) override;
    virtual void PostUserTrackingAction(const G4Track* const track) override;

    bool GetFirstStepInVolume() const { return firstStepInVolume; }
    void SetFirstStepInVolume(const bool b) { firstStepInVolume = b; }


  private:

  	bool firstStepInVolume = true;

};

#endif

