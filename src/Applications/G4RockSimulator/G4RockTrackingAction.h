#ifndef G4RockTrackingAction_h
#define G4RockTrackingAction_h 1

#include <G4UserTrackingAction.hh>

class G4Track;

class G4RockTrackingAction : public G4UserTrackingAction {
	public:
		G4RockTrackingAction() { }
		~G4RockTrackingAction() { }

		virtual void PreUserTrackingAction(const G4Track*  track) override;
		virtual void PostUserTrackingAction(const G4Track* track) override;

		bool GetIsMuonDecay() { return fIsMuonDecay; }
		void SetIsMuonDecay() { fIsMuonDecay = true; }

	private:

		bool fIsMuonDecay;

};

#endif

