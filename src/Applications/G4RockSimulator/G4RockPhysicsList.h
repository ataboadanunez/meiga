// definiton of the G4RockPhysicsList class

#ifndef G4RockPhysicsList_h
#define G4RockPhysicsList_h 1

#include "globals.hh"
#include "G4VModularPhysicsList.hh"


class G4VPhysicsConstructor;
class PhysicsListMessenger;

class StepMax;
class OpticalPhysics;


class G4RockPhysicsList: public G4VModularPhysicsList
{
  public:

	G4RockPhysicsList(G4String);
	virtual ~G4RockPhysicsList();

	void SetCuts();
	void SetCutForGamma(G4double);
	void SetCutForElectron(G4double);
	void SetCutForPositron(G4double);

	void SetStepMax(G4double);
	StepMax* GetStepMaxProcess();
	void AddStepMax();

	/// Remove specific physics from physics list.
	void RemoveFromPhysicsList(const G4String&);

	/// Make sure that the physics list is empty.
	void ClearPhysics();

	virtual void ConstructParticle();
	virtual void ConstructProcess();

	// Turn on or off the absorption process
	void SetAbsorption(G4bool);

	void SetNbOfPhotonsCerenkov(G4int);

	void SetVerbose(G4int);

private:

	G4double fCutForGamma;
	G4double fCutForElectron;
	G4double fCutForPositron;

	StepMax* fStepMaxProcess;

	OpticalPhysics* fOpticalPhysics;

	//PhysicsListMessenger* fMessenger;

	G4bool fAbsorptionOn;
	
	G4VMPLData::G4PhysConstVectorData* fPhysicsVector;

};

#endif
