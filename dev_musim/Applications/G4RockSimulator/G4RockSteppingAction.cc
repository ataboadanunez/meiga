// implementation of the G4RockSteppingAction class
#include "G4RockSimulator.h"
#include "G4RockSteppingAction.h"
#include "G4RockDetectorConstruction.h"
#include "G4RockEventAction.h"
#include "G4RockTrackingAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"

#include "SimData.h"
#include "SiPMSimData.h"
#include "Detector.h"
#include "Module.h"

G4RockSteppingAction::G4RockSteppingAction(const G4RockDetectorConstruction* det, G4RockEventAction* G4event, std::ofstream* ofile, Event& MSevent)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(G4event),
    fOutputFile(ofile),
    fEvent(MSevent)
{
}

G4RockSteppingAction::~G4RockSteppingAction()
{
}

void
G4RockSteppingAction::UserSteppingAction(const G4Step* step)
{
    
	SimData& simData = fEvent.GetSimData();
	Detector& detector = fEvent.GetDetector();
	Module& module = detector.GetModule();
	double barLength = module.GetBarLength();

	const G4String& particleName = step->GetTrack()->GetDefinition()->GetParticleName();
	auto track = step->GetTrack();
	const double time = step->GetPreStepPoint()->GetGlobalTime() / (1*ns);
	trackVolume = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();

	std::vector<double>* sipmTrace = fEventAction->GetSiPMTrace();

	// track only primaries
	// if (track->GetParentID() == 0) {
	// 	if (step->GetTotalEnergy() == 0)
	// 		cout << "Particle " << particleName << " stopped at position " << step->
	// }

	/***

	if (particleName == "opticalphoton") {
		auto preVolume = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
		auto postVolume = step->GetPostStepPoint()->GetPhysicalVolume()->GetName();
		auto photonEnergy = track->GetTotalEnergy() / (1*eV);
		auto trackPosition = track->GetPosition();

		G4VPhysicalVolume* const current = step->GetTrack()->GetTouchable()->GetVolume(0);
		G4String currentVolName = current->GetName();
		unsigned int volumeID = current->GetCopyNo();
		currentVolName = currentVolName.substr(0, currentVolName.find('_'));
		//std::cout << "[DEBUG] G4RockSteppingAction: Photon detected in volume " << currentVolName << " " << volumeID << std::endl;
		if (currentVolName == "SiPM" && IsDetected(photonEnergy)) {
			//G4cout << "[DEBUG] G4RockSteppingAction: Making Traces For SiPM " << volumeID << G4endl;
			//fEventAction->MakeSiPMTraces(volumeID);

			//std::vector<double>* sipmTracei = fEventAction->GetSiPMTrace(volumeID);
			if (volumeID == 0) {
				fEventAction->fNpe0 += 1;
				fEventAction->fSiPMTrace0->push_back(time);
			}
			else if (volumeID == 1) {
				fEventAction->fNpe1 += 1;
				fEventAction->fSiPMTrace1->push_back(time);
			}
			else if (volumeID == 2) {
				fEventAction->fNpe2 += 1;
				fEventAction->fSiPMTrace2->push_back(time);
			}
			else if (volumeID == 3) {
				fEventAction->fNpe3 += 1;
				fEventAction->fSiPMTrace3->push_back(time);
			}

			(*fOutputFile) << time << " ";
			sipmTrace->push_back(time);
			


			//fEventAction->addPhoton(time);

		}
	} 
	***/
}

G4bool
G4RockSteppingAction::IsDetected(G4double energy)
{
  /*
  this function returns if "photon" was detected by the SiPM
  based on its photo detection efficiency (not same as quantum eff.)
  it is tabulated (see data-sheet) and depends on photon wavelength (energy)
  */
  G4double waveLength = 1240. / energy;
  G4double rand = G4UniformRand();
  G4bool isdetected = false;

  
  if (waveLength >= 300. && waveLength < 350.)
    rand < 0.07 ? isdetected = true : isdetected = false;
  
  else if (waveLength >= 350. && waveLength < 400.)
    rand < 0.25 ? isdetected = true : isdetected = false;
  
  else if (waveLength >= 400. && waveLength < 450.)
    rand < 0.37 ? isdetected = true : isdetected = false;
  
  else if (waveLength >= 450. && waveLength < 500.)
    rand < 0.40 ? isdetected = true : isdetected = false;
  
  else if (waveLength >= 500. && waveLength < 550.)
    rand < 0.35 ? isdetected = true : isdetected = false;

  else if (waveLength >= 550. && waveLength < 600.)
    rand < 0.29 ? isdetected = true : isdetected = false;

  else if (waveLength >= 600. && waveLength < 650.)
    rand < 0.23 ? isdetected = true : isdetected = false;

  else if (waveLength >= 650. && waveLength < 700.)
    rand < 0.18 ? isdetected = true : isdetected = false;

  else if (waveLength >= 700. && waveLength < 750.)
    rand < 0.13 ? isdetected = true : isdetected = false;

  else if (waveLength >= 750. && waveLength < 800.)
    rand < 0.10 ? isdetected = true : isdetected = false;

  else if (waveLength >= 800. && waveLength < 850.)
    rand < 0.07 ? isdetected = true : isdetected = false;

  else if (waveLength >= 850. && waveLength < 900.)
    rand < 0.04 ? isdetected = true : isdetected = false;
  
  else
   isdetected = false;

  return isdetected;
}

