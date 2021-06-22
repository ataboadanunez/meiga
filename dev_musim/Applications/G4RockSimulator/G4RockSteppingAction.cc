// implementation of the G4RockSteppingAction class
#include "G4RockSimulator.h"
#include "G4RockSteppingAction.h"
#include "G4RockDetectorConstruction.h"
#include "G4RockEventAction.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"

G4RockSteppingAction::G4RockSteppingAction(const G4RockDetectorConstruction* det, G4RockEventAction* event, std::ofstream* ofile)
	: G4UserSteppingAction(),
		fDetectorConstruction(det),
		fEventAction(event),
    fOutputFile(ofile)
{
}

G4RockSteppingAction::~G4RockSteppingAction()
{
}

void
G4RockSteppingAction::UserSteppingAction(const G4Step* step)
{

		const G4String& particleName = step->GetTrack()->GetDefinition()->GetParticleName();
		auto track = step->GetTrack();
		const double time = step->GetPreStepPoint()->GetGlobalTime() / (1*ns);
		trackVolume = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
		std::vector<double> sipmTrace = fEventAction->GetSiPMTrace();
    G4bool photonKilled = false;

		if (particleName == "opticalphoton") {
			auto preVolume = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
			auto postVolume = step->GetPostStepPoint()->GetPhysicalVolume()->GetName();
      auto photonEnergy = track->GetTotalEnergy() / (1*eV);
      auto trackPosition = track->GetPosition();
      //G4double trackPosX = trackPosition.getX() / (1.*mm);
      //G4double trackPosY = trackPosition.getY() / (1.*mm);
      //G4double trackPosZ = trackPosition.getZ() / (1.*mm);
      
      if (postVolume == "SiPM_back") {
        track->SetTrackStatus(fStopAndKill);
        photonKilled = true;
      }


			if (postVolume == "SiPM" && IsDetected(photonEnergy)){

        if (photonKilled) 
          G4cout << "[DEBUG] G4RockSteppingAction Photon was supposed to be killed " << G4endl;
				// dump time to output file
        // compute distance to SiPM
        // info to compute distance to SiPM
        G4TouchableHandle theTouchable = step->GetPreStepPoint()->GetTouchableHandle();
        G4ThreeVector origin = step->GetPreStepPoint()->GetPosition();
        G4ThreeVector sipmPos = theTouchable->GetHistory()->GetTopTransform().TransformPoint(origin);
        // optical photon arrived to SiPM
        //G4cout << "[DEBUG] SteppingAction: OpticalPhoton collected by SiPM: time = " << time << " " << " " << trackPosX << " " << trackPosY << " " << trackPosZ << G4endl;
        (*fOutputFile) << time << " ";
				//(*fEventAction->fRunAction->outFile) << time << " ";
				sipmTrace.push_back(time);
				
				// for (int i=0; i<sipmTrace.size(); i++)
				// 	G4cout << sipmTrace[i] << G4endl;
			} 
			
		}
}

// reescribir esto de mejor forma... -.-'
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