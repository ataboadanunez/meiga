// definition of the G4MuDecRunAction class

#ifndef G4MuDecRunAction_h
#define G4MuDecRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
// #include "g4root.hh"

#include <fstream>
#include <iostream>
#include <string>


class G4Run;


class G4MuDecRunAction : public G4UserRunAction
{
  public:
    G4MuDecRunAction();
    virtual ~G4MuDecRunAction();

    virtual void BeginOfRunAction(const G4Run* aRun);
    virtual void EndOfRunAction(const G4Run* aRun);

    std::ofstream* outFile;


  private:
};
#endif 
