#include "G4MPMTAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "OptDeviceSimData.h"
#include "OptDevice.h"

#include <G4Step.hh>
#include <G4TouchableHistory.hh>
#include <G4RunManager.hh>
#include <G4OpticalPhoton.hh>

using namespace std;

G4MPMTAction::G4MPMTAction(const G4String& name, const G4int dId, const G4int oId, Event& theEvent) :
    G4VSensitiveDetector(name),
    fEvent(theEvent),
    fDetectorId(dId),
    fOptDeviceId(oId)

  { 
    
    cout << "[INFO] G4Models::G4MPMTAction: Registering PMT " << name << endl;
    SimData& simData = fEvent.GetSimData();
    DetectorSimData& detSimData = simData.GetDetectorSimData(fDetectorId);
    detSimData.MakeOptDeviceSimData(fOptDeviceId);
    OptDeviceSimData& OptDeviceSimData = detSimData.GetOptDeviceSimData(fOptDeviceId);

    fPETimeDistribution = OptDeviceSimData.PETimeDistributionRange();

  }

void
G4MPMTAction::Initialize(G4HCofThisEvent* const /*hce*/)
{
  
  NumCerenkovPhotons = 0;
  fPETime = new std::vector<double>();

}

void
G4MPMTAction::EndOfEvent(G4HCofThisEvent* const /*hce*/)
{
  fEvent.GetSimData().GetDetectorSimData(fDetectorId).GetOptDeviceSimData(fOptDeviceId).AddPETimeDistribution(fPETime);
  //fPETimeDistribution->push_back(fPETime);

  cout << "Number of Cerenkov Photons at PMT = " << NumCerenkovPhotons << endl;

}

G4bool
G4MPMTAction::ProcessHits(G4Step* const step, G4TouchableHistory* const /*rOHist*/)
{
  // reject particle in case it is not a photon

  if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    return true;

  NumCerenkovPhotons += 1;


  SimData& simData = fEvent.GetSimData();
  auto& pmt = fEvent.GetDetector(fDetectorId).GetOptDevice(fOptDeviceId);
  const double time = step->GetPreStepPoint()->GetGlobalTime() / CLHEP::second;
  double energy = step->GetPreStepPoint()->GetKineticEnergy() / CLHEP::eV;

  // cout << "[DEBUG] G4Models::G4MPMTAction: Photon with Energy " << energy << " and time " << time << " arrived to PMT" << endl;

  if (time >= 1*CLHEP::second) {
    return true;
  }

  if (pmt.IsPhotonDetected(energy)) 
  {
    DetectorSimData& detSimData = simData.GetDetectorSimData(fDetectorId);
    OptDeviceSimData& OptDeviceSimData = detSimData.GetOptDeviceSimData(fOptDeviceId);
    //cout << "[DEBUG] G4Models::G4MPMTAction: Photon was detected!" << endl;
    OptDeviceSimData.AddPhotonEnergy(energy);
    OptDeviceSimData.AddPhotonTime(time);
    fPETime->push_back(time);

  }
  
  return true;

}
  


