// implementation of the G4RockRunAction class

#include "G4Timer.hh"
#include "G4Run.hh"
#include "g4root.hh"
#include "G4AccumulableManager.hh"
 
#include "G4RockRunAction.h"
//#include "histosRun.hh"

//#include <TFile.h>

G4RockRunAction::G4RockRunAction()
 : G4UserRunAction()
{
  G4cout << "...G4RockRunAction..." << G4endl;
}


G4RockRunAction::~G4RockRunAction()
{}


void 
G4RockRunAction::BeginOfRunAction(const G4Run* aRun)
{
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  outFile = new std::ofstream();
  outFile->open("SiPMTraces.dat"); // mover a config file
}


void 
G4RockRunAction::EndOfRunAction(const G4Run* aRun)
{
  //fTimer->Stop();
  G4cout << "number of event = " << aRun->GetNumberOfEvent() << G4endl;
  outFile->close();
}
