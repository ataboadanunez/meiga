// implementation of the G4RockEventAction class

#include "G4RockEventAction.h"

#include "G4Track.hh"

G4RockEventAction::G4RockEventAction(G4RockRunAction* runAction, G4RockPrimaryGeneratorAction* priGenAction)
  : G4UserEventAction(),
    fRunAction(runAction),
    fPriGenAction(priGenAction)
{
  G4cout << "...G4RockEventAction..." << G4endl;
  traces = new std::vector<std::vector<double>*>();

  //fSiPMTrace = new std::vector<double>(fTraceLength);

}

G4RockEventAction::~G4RockEventAction()
{

}

void
G4RockEventAction::BeginOfEventAction(const G4Event*)
{

  fSiPMTrace = new std::vector<double>();
  fSiPMTrace0 = new std::vector<double>();
  fSiPMTrace1 = new std::vector<double>();
  fSiPMTrace2 = new std::vector<double>();
  fSiPMTrace3 = new std::vector<double>();
  
  fNpe0 = fNpe1 = fNpe2 = fNpe3 = 0;
  // aqui inicializamos la traza con ceros
  // cada traza es llenada con ceros al inicio de cada evento (particula inyectada)

  //for (long unsigned int  t=0; t < fTraceLength; t++) {
     //fSiPMTrace[t] = 0;
  //}
}

void 
G4RockEventAction::EndOfEventAction(const G4Event* )
{
  traces->push_back(fSiPMTrace);

  if (fNpe0) {
  	G4cout << "NPE0 " << fNpe0 << " ";
  	for (int i=0; i<fNpe0; i++) {
  		G4cout << fSiPMTrace0->at(i) << " ";
  	}
  	G4cout << G4endl;
  }

  if (fNpe1) {
  	G4cout << "NPE1 " << fNpe1 << " ";
  	for (int i=0; i<fNpe1; i++) {
  		G4cout << fSiPMTrace1->at(i) << " ";
  	}
  	G4cout << G4endl;
  }

  if (fNpe2) {
  	G4cout << "NPE2 " << fNpe2 << " ";
  	for (int i=0; i<fNpe2; i++) {
  		G4cout << fSiPMTrace2->at(i) << " ";
  	}
  	G4cout << G4endl;
  }

  if (fNpe3) {
  	G4cout << "NPE3 " << fNpe3 << " ";
  	for (int i=0; i<fNpe3; i++) {
  		G4cout << fSiPMTrace3->at(i) << " ";
  	}
  	G4cout << G4endl;
  }

  //G4cout << "PhotoElectrons " << fNpe0 << " " << fNpe1 << " " << fNpe2 << " " << fNpe3 << G4endl;

  // escribe traza en output file
  // for (long unsigned int t=0; t < fTraceLength; t++) {
  //   G4cout << (&fSiPMTrace)[t] << G4endl;
  // }
  // (*fRunAction->outFile) << G4endl;
}


void
G4RockEventAction::MakeSiPMTraces(unsigned int id)
{
	fTracesMap.emplace(id, GetSiPMTrace(id));
}