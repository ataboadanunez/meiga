#ifndef G4BarEventAction_h 
#define G4BarEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "G4BarRunAction.h"
#include "G4BarPrimaryGeneratorAction.h"

#include <string>
#include <vector>

#include <sstream>
#include <fstream>
#include <iostream>


class G4BarEventAction : public G4UserEventAction
{
  public:
    G4BarEventAction(std::ofstream* out, G4BarRunAction* runAction, G4BarPrimaryGeneratorAction* primaryGenAction);
    virtual ~G4BarEventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);

    G4BarRunAction* fRunAction;
    G4BarPrimaryGeneratorAction* fPriGenAction;
    std::ofstream* fOutputFile;
    
    std::vector<std::vector<double>*>* GetTraces() { return traces; }

    int fNpe0;
    int fNpe1;
    
    std::vector<double>* fPMTTrace0;
    std::vector<double>* fPMTTrace1;

  private:
  	
    std::vector<std::vector<double>*>* traces;
    std::map<int, std::vector<double>*> fTracesMap;

    int fTraceLength = 100;
		 
    
};

#endif

