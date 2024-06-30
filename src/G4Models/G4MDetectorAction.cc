#include "G4MDetectorAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "CorsikaUtilities.h"
#include "Logger.h"

#include <G4Step.hh>
#include <G4Track.hh>
#include <G4TouchableHistory.hh>


using namespace std;

G4MDetectorAction::G4MDetectorAction(const G4String& name, const G4int dId, Event& theEvent) : 
	G4VSensitiveDetector(name),
	fEvent(theEvent),
	fDetectorId(dId)
	{
		Logger::Print("Registering Sensitive Detector: " + name, INFO, "G4MDetectorAction");
	}


void 
G4MDetectorAction::Initialize(G4HCofThisEvent* const /*hce*/)
{
	fEdep = 0;
}

void
G4MDetectorAction::EndOfEvent(G4HCofThisEvent* const /*hce*/)
{
	DetectorSimData& detSimData = fEvent.GetSimData().GetDetectorSimData(fDetectorId);
	// get current particle in the event
	Particle currentParticle = fEvent.GetSimData().GetCurrentParticle();
	int partId = currentParticle.GetParticleId();
	Particle::Component particleComp = currentParticle.GetComponent(/*particleType*/);
	
	detSimData.SetEnergyDeposit(fEdep);
	detSimData.SetEnergyDeposit(particleComp, fEdep);

	ostringstream msg;
	msg << "Energy Deposit at detector " << fDetectorId << " = " << fEdep;
	Logger::Print(msg, INFO, "G4MDetectorAction");
}

G4bool
G4MDetectorAction::ProcessHits(G4Step* const step, G4TouchableHistory* const /*rOHist*/)
{
	auto track = step->GetTrack();
	auto trackId = track->GetTrackID();
	// get energy deposit of primaries
	if (trackId == 1) {
		fEdep += step->GetTotalEnergyDeposit();
	}

	return true;
}

G4VSensitiveDetector* G4MDetectorAction::Clone() const
{
	return nullptr;
}
