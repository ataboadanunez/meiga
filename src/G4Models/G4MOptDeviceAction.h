#ifndef G4MOptDeviceAction_h 
#define G4MOptDeviceAction_h 1

#include "Event.h"
#include <G4VSensitiveDetector.hh>


class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class G4MOptDeviceAction : public G4VSensitiveDetector {
	public:
		G4MOptDeviceAction(const G4String& name, const G4int dId, const G4int oId, Event& theEvent);
		virtual ~G4MOptDeviceAction() { }

		virtual void Initialize(G4HCofThisEvent* const /*hce*/) override;
		virtual G4bool ProcessHits(G4Step* const step, G4TouchableHistory* const rOhist) override;
		virtual void EndOfEvent(G4HCofThisEvent* const /*hce*/) override;

	private:
		
		Event& fEvent;

		int fDetectorId = 0;
		int fOptDeviceId = 0;

		std::vector<double> fPETime;
		// std::vector<std::vector<double>*>* fPETimeDistribution;
};


#endif