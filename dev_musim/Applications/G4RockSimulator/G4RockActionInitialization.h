// definition of the G4RockActionInitialization class

#ifndef G4RockActionInitialization_h
#define G4RockActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
//#include "ParticleInjector.hh"


class G4RockDetectorConstruction;

class G4RockActionInitialization : public G4VUserActionInitialization
{
  public:
    G4RockActionInitialization(const G4RockDetectorConstruction* det);
    virtual ~G4RockActionInitialization();

    //virtual void BuildForMaster() const;
    virtual void Build() const;

    //virtual G4VSteppingVerbose* InitializeSteppingVerbose() const;
    //ParticleInjector* fParticleInjector;
	private:
		const G4RockDetectorConstruction* fDetectorConstruction;

};

#endif
