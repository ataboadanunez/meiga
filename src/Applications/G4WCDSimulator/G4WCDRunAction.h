// definition of the G4WCDRunAction class

#ifndef G4WCDRunAction_h
#define G4WCDRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
#include "g4root.hh"

#include <fstream>
#include <iostream>
#include <string>


class G4Run;


class G4WCDRunAction : public G4UserRunAction
{
  public:
    G4WCDRunAction();
    virtual ~G4WCDRunAction();

    virtual void BeginOfRunAction(const G4Run* aRun);
    virtual void EndOfRunAction(const G4Run* aRun);

    std::ofstream* outFile;


  private:
};
#endif 
