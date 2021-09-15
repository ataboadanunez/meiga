#include "G4BarEventAction.h"
#include "G4Track.hh"

G4BarEventAction::G4BarEventAction(std::ofstream* outputFile, G4BarRunAction* runAction, G4BarPrimaryGeneratorAction* priGenAction)
  : G4UserEventAction(),
    fRunAction(runAction),
    fPriGenAction(priGenAction),
    fOutputFile(outputFile)
{
  
  G4cout << "...G4BarEventAction..." << G4endl;

  traces = new std::vector<std::vector<double>*>();

 }

G4BarEventAction::~G4BarEventAction()
{

}

void
G4BarEventAction::BeginOfEventAction(const G4Event*)
{

  fPMTTrace0 = new std::vector<double>();
  fPMTTrace1 = new std::vector<double>();
  
  fNpe0 = fNpe1 = 0;

}

void 
G4BarEventAction::EndOfEventAction(const G4Event* )
{

	G4ThreeVector particlePosition = fPriGenAction->fParticlePosition;
	double posX = particlePosition[0] / cm;

	(*fOutputFile) << posX << " " << fNpe0 << " " << fNpe1 << "\n";

  // if (fNpe0) {
  // 	G4cout << "NPE0 " << fNpe0 << " ";
  // 	for (int i=0; i<fNpe0; i++) {
  // 		G4cout << fPMTTrace0->at(i) << " ";
  // 	}
  // 	G4cout << G4endl;
  // }

  // if (fNpe1) {
  // 	G4cout << "NPE1 " << fNpe1 << " ";
  // 	for (int i=0; i<fNpe1; i++) {
  // 		G4cout << fPMTTrace1->at(i) << " ";
  // 	}
  // 	G4cout << G4endl;
  // }

}