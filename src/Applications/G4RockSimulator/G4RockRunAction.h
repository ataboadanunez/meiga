// definition of the G4RockRunAction class

#ifndef G4RockRunAction_h
#define G4RockRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
// #include "g4root.hh"

#include <fstream>
#include <iostream>
#include <string>


class G4Run;


class G4RockRunAction : public G4UserRunAction
{
  public:
    G4RockRunAction();
    virtual ~G4RockRunAction();

    virtual void BeginOfRunAction(const G4Run* aRun);
    virtual void EndOfRunAction(const G4Run* aRun);

    std::ofstream* outFile;


  private:
};
#endif 
