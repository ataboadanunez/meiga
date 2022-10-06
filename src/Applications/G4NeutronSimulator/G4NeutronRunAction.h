// definition of the G4NeutronRunAction class

#ifndef G4NeutronRunAction_h
#define G4NeutronRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
#include "g4root.hh"

#include <fstream>
#include <iostream>
#include <string>


class G4Run;


class G4NeutronRunAction : public G4UserRunAction
{
  public:
    G4NeutronRunAction();
    virtual ~G4NeutronRunAction();

    virtual void BeginOfRunAction(const G4Run* aRun);
    virtual void EndOfRunAction(const G4Run* aRun);

    std::ofstream* outFile;


  private:
};
#endif 
