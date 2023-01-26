#ifndef PrimaryGenerator_h
#define PrimaryGenerator_h 1

#include "Event.h"
#include "SimData.h"
#include "EcoMug.h"

class PrimaryGenerator
{

public:
	PrimaryGenerator(Event& theEvent);
	virtual ~PrimaryGenerator();

	void GeneratePrimaryParticle();

private:

	

	std::vector<double> fParticlePosition;
	std::vector<double> fParticleDirection;
	int fParticleId;
	
	SimData::InjectionMode injMode;
	// for muon generator using EcoMug
	EcoMug fMuonGen;
	bool fUseEcoMug;
	
	Event& fEvent;

};


#endif