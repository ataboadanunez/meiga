// definition of the G4RockSimulator class
#ifndef G4RockSimulator_h
#define G4RockSimulator_h 1

#include <sstream>
#include <fstream>
#include <iostream>
#include "Particle.h"

class G4RunManager;
class G4StationPrimaryGenerator;



class G4RockSimulator 
{
  public:
    G4RockSimulator();
    virtual ~G4RockSimulator();
    static Particle currentParticle;
    std::ofstream fOutputFile;

  private:

  	friend class G4RockDetectorConstructor;
  	friend class G4RockPrimaryGenerator;

};

#endif
