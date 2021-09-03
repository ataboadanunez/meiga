#ifndef G4BarSimulator_h
#define G4BarSimulator_h 1

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

#include "Particle.h"
#include "Detector.h"

class G4RunManager;
class G4StationPrimaryGenerator;

class G4BarSimulator 
{
  public:
    G4BarSimulator();
    virtual ~G4BarSimulator();

    void ReadConfiguration(std::string filename);
    //void ConstructTraces(Detector& det);

    static Particle currentParticle;
    std::ofstream* fOutputFile;
    std::ofstream* fParticleInfo;
    static G4BarSimulator* fG4BarSimulator;

  private:

  	std::string fInputFile, fModuleListFile;
		int geometry, trajectories, verbosity;
		double injectionRadius;
		//Estos strings tendrían que ser enum/tipo razonable
		std::string renderFile, injectionRadiusUnit;

  	friend class G4BarDetectorConstructor;
  	friend class G4BarPrimaryGenerator;

};

#endif
