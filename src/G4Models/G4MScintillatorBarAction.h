#ifndef G4ScintillatorBarAction_h
#define G4ScintillatorBarAction_h 1

#include "Event.h"
#include <G4VSensitiveDetector.hh>

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class G4MScintillatorBarAction : public G4VSensitiveDetector {
	
	public:
		G4MScintillatorBarAction(const G4String& name, const G4int dId, const G4int barId, Event& theEvent);
		virtual ~G4MScintillatorBarAction() { }

		virtual void Initialize(G4HCofThisEvent* const /*hce*/) override;
		virtual G4bool ProcessHits(G4Step* const step, G4TouchableHistory* const rOhist) override;
		virtual void EndOfEvent(G4HCofThisEvent* const /*hce*/) override;

	private:

		Event& fEvent;
		int fDetectorId;
		int fBarId;
		double fBarEdep;

};

#endif