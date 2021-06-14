// implementation of the G4RockEventAction class

#include "G4RockEventAction.h"

#include "G4Track.hh"

G4RockEventAction::G4RockEventAction(G4RockRunAction* runAction, G4RockPrimaryGeneratorAction* priGenAction)
  : G4UserEventAction(),
    fRunAction(runAction),
    fPriGenAction(priGenAction)
{
  G4cout << "...G4RockEventAction..." << G4endl;
  fSiPMTrace.reserve(fTraceLength);

}

G4RockEventAction::~G4RockEventAction()
{

}

void
G4RockEventAction::BeginOfEventAction(const G4Event* event)
{
  // aqui inicializamos la traza con ceros
  // cada traza es llenada con ceros al inicio de cada evento (particula inyectada)

  for (long unsigned int  t=0; t < fSiPMTrace.size(); t++) {
    fSiPMTrace[t] = 0;
  }
}

void 
G4RockEventAction::EndOfEventAction(const G4Event* event)
{
  // // escribe traza en output file
  // for (long unsigned int t=0; t < fSiPMTrace.size(); t++) {
  //   (*fRunAction->outFile) << fSiPMTrace[t] << " ";
  // }
  // (*fRunAction->outFile) << G4endl;
}
