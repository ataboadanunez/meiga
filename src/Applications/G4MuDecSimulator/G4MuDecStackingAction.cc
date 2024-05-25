#include "G4MuDecStackingAction.h"
#include "SimData.h"
#include "OptDevice.h"

#include <G4OpticalPhoton.hh>

G4MuDecStackingAction::G4MuDecStackingAction(Event& theEvent) : 
  G4UserStackingAction(),
  fEvent(theEvent)

{
}

G4MuDecStackingAction::~G4MuDecStackingAction() 
{ 
}


G4ClassificationOfNewTrack
G4MuDecStackingAction::ClassifyNewTrack(const G4Track* const track)
{
  // if particle is no a photon, track it
  if (track->GetDefinition() != G4OpticalPhoton::OpticalPhoton())
    return fWaiting;

  // here we are left with optical photons
  SimData& simData = fEvent.GetSimData();
  const auto simMode = simData.GetSimulationMode();

  // if not (fast simulation mode), kill Cerenkov photons according 
  // energy and the PMT quantum efficiency
  if (simMode == SimData::SimulationMode::eFull)
    return fWaiting;

  // get PMT
  OptDevice pmt = fEvent.GetDetector().GetOptDevice(OptDevice::ePMT);
  double energy = track->GetKineticEnergy() / CLHEP::eV;

  bool isDetected = pmt.IsPhotonDetected(energy);

  return isDetected ? fWaiting : fKill;

}