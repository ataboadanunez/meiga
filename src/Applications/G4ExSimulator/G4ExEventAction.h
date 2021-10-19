// definition of G4ExEventAction class
#ifndef G4ExEventAction_h 
#define G4ExEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "G4ExRunAction.h"
#include "G4ExPrimaryGeneratorAction.h"

#include <string>
#include <vector>


class G4ExEventAction : public G4UserEventAction
{
  public:
    G4ExEventAction(G4ExRunAction* runAction, G4ExPrimaryGeneratorAction* primaryGenAction);
    virtual ~G4ExEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);

    G4ExRunAction* fRunAction;
    G4ExPrimaryGeneratorAction* fPriGenAction;
    
    std::vector<double>* GetSiPMTrace() { return fSiPMTrace; }
    std::vector<double>* GetSiPMTrace(const int id) { return fTracesMap[id]; }
    void MakeSiPMTraces(unsigned int id);

    std::vector<std::vector<double>*>* GetTraces() { return traces; }

    void addPhoton( double time ) { fSiPMTrace->push_back(time); }

    int fNpe0;
    int fNpe1;
    int fNpe2;
    int fNpe3;
    
    std::vector<double>* fSiPMTrace0;
    std::vector<double>* fSiPMTrace1;
    std::vector<double>* fSiPMTrace2;
    std::vector<double>* fSiPMTrace3;

  private:
    std::vector<double>* fSiPMTrace;
    std::vector<std::vector<double>*>* traces;
    std::map<int, std::vector<double>*> fTracesMap;

    int fTraceLength = 100; 
};

#endif

