#ifndef G4BarRunAction_h
#define G4BarRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
#include "g4root.hh"

#include <fstream>
#include <iostream>
#include <string>


class G4Run;


class G4BarRunAction : public G4UserRunAction
{
  public:
    G4BarRunAction();
    virtual ~G4BarRunAction();

    virtual void BeginOfRunAction(const G4Run* aRun);
    virtual void EndOfRunAction(const G4Run* aRun);

    std::ofstream* outFile;


  private:
};
#endif 
