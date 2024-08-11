// definition of the G4ExRunAction class

#ifndef G4ExRunAction_h
#define G4ExRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"

#include <fstream>
#include <iostream>
#include <string>


class G4Run;


class G4ExRunAction : public G4UserRunAction
{
  public:
    G4ExRunAction();
    virtual ~G4ExRunAction();

    virtual void BeginOfRunAction(const G4Run* aRun);
    virtual void EndOfRunAction(const G4Run* aRun);

    std::ofstream* outFile;


  private:
};
#endif 
