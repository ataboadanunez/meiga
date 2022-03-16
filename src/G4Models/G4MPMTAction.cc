#include "G4MPMTAction.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "OptDeviceSimData.h"
#include "OptDevice.h"
#include "CorsikaUtilities.h"

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
    
    cout << "[INFO] G4Models::G4MPMTAction: Registering Sensitive Detector " << name << endl;
    SimData& simData = fEvent.GetSimData();
    DetectorSimData& detSimData = simData.GetDetectorSimData(fDetectorId);
    detSimData.MakeOptDeviceSimData(fOptDeviceId);
    OptDeviceSimData& OptDeviceSimData = detSimData.GetOptDeviceSimData(fOptDeviceId);

    fPETimeDistribution = OptDeviceSimData.PETimeDistributionRange();

  }

void
G4MPMTAction::Initialize(G4HCofThisEvent* const /*hce*/)
{
  

  NumPE = 0;
  fPETime = new std::vector<double>();

}

void
G4MPMTAction::EndOfEvent(G4HCofThisEvent* const /*hce*/)
{
  fEvent.GetSimData().GetDetectorSimData(fDetectorId).GetOptDeviceSimData(fOptDeviceId).AddPETimeDistribution(fPETime);
  // gets current particle in the event to stream the number of PE for each particle
  Particle currentParticle = fEvent.GetSimData().GetCurrentParticle();
  int partId = currentParticle.GetParticleId();
  // time distribution for different particles

  Particle::Type particleType = Corsika::CorsikaToPDG(partId);
  Particle::Component particleComponent = currentParticle.GetComponent(particleType);

  fEvent.GetSimData().GetDetectorSimData(fDetectorId).GetOptDeviceSimData(fOptDeviceId).AddPETimeDistribution(particleComponent, fPETimeComp);

  fPETimeComp.clear();

}

G4bool
G4MPMTAction::ProcessHits(G4Step* const step, G4TouchableHistory* const /*rOHist*/)
{
  
  // reject particle in case it is not a photon
  if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    return true;

  // get time and energy of photons
  const double time = step->GetPreStepPoint()->GetGlobalTime() / CLHEP::second;
  if (time >= 1*CLHEP::second)
    return true;

  auto& pmt = fEvent.GetDetector(fDetectorId).GetOptDevice(fOptDeviceId);
  double energy = step->GetPreStepPoint()->GetKineticEnergy() / CLHEP::eV;
  // kill if photon energy is out of PMT range
  if (energy < pmt.GetOpticalRange()[0]  || energy > pmt.GetOpticalRange()[1]) 
    return true; 

  SimData& simData = fEvent.GetSimData();
  if (simData.GetSimulationMode() == "FULL_SIMULATION") {
    
    // kill according to PMT quantum efficiency
    if (!pmt.IsPhotonDetected(energy)) 
      return true;
  }
  
  
  // add photon time to SimData
  fPETime->push_back(time);
  fPETimeComp.push_back(time);
  return true;

}
  


