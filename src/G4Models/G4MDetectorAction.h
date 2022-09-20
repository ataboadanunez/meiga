#ifndef G4DetectorAction_h
#define G4DetectorAction_h 1

#include "Event.h"
#include <G4VSensitiveDetector.hh>

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class G4MDetectorAction : public G4VSensitiveDetector {
	
	public:
		G4MDetectorAction(const G4String& name, const G4int dId, Event& theEvent);
		virtual ~G4MDetectorAction() { }

		virtual void Initialize(G4HCofThisEvent* const /*hce*/) override;
		virtual G4bool ProcessHits(G4Step* const step, G4TouchableHistory* const rOhist) override;
		virtual void EndOfEvent(G4HCofThisEvent* const /*hce*/) override;

	private:

		Event& fEvent;
		int fDetectorId;

		//std::vector<double> fEnergyDeposit;
		double fEdep;

};

#endif