#ifndef G4BarSteppingAction_h
#define G4BarSteppingAction_h 1
#include <iostream>
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Event.h"


class G4BarDetectorConstruction;
class G4BarEventAction;

/*
	Stepping action class.
	UserSteppingAction collects arrival time of optical photons
	at the photodetector (PMT)
*/

class G4BarSteppingAction : public G4UserSteppingAction
{
	public:

		G4BarSteppingAction(const G4BarDetectorConstruction* det, G4BarEventAction* event, Event& theEvent);
		virtual ~G4BarSteppingAction();
		virtual void UserSteppingAction(const G4Step* step);

		bool IsDetected(double pe);
	
	private:
		const G4BarDetectorConstruction* fDetectorConstruction;
		G4BarEventAction* fEventAction;
		Event& fEvent;
};

#endif