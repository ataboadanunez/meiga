// definition of the G4RockSimulator class
#ifndef G4RockSimulator_h
#define G4RockSimulator_h 1

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

#include "Particle.h"
#include "Detector.h"

class G4RunManager;
class G4StationPrimaryGenerator;

class G4RockSimulator 
{
  public:
    G4RockSimulator();
    virtual ~G4RockSimulator();

    void ReadConfiguration(std::string filename);
    //void ConstructTraces(Detector& det);

    static Particle currentParticle;
    std::ofstream* fOutputFile;
    std::ofstream* fParticleInfo;
    static G4RockSimulator* fG4RockSimulator;

  private:

  	std::string fInputFile, fModuleListFile;
		int geometry, trajectories, verbosity;
		double injectionRadius;
		//Estos strings tendrían que ser enum/tipo razonable
		std::string renderFile, injectionRadiusUnit;

  	friend class G4RockDetectorConstructor;
  	friend class G4RockPrimaryGenerator;

};

#endif
