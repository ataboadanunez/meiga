// definition of the G4CasposoRunAction class

#ifndef G4CasposoRunAction_h
#define G4CasposoRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
#include "g4root.hh"

#include <fstream>
#include <iostream>
#include <string>


class G4Run;


class G4CasposoRunAction : public G4UserRunAction
{
  public:
    G4CasposoRunAction();
    virtual ~G4CasposoRunAction();

    virtual void BeginOfRunAction(const G4Run* aRun);
    virtual void EndOfRunAction(const G4Run* aRun);

    std::ofstream* outFile;


  private:
};
#endif 
