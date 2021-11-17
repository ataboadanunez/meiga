// implementation of the ParticleInjector class
#include "ParticleInjector.h"

#include <CLHEP/Random/Randomize.h>

#include "G4SystemOfUnits.hh" // we should convert to G4 units later

using CLHEP::RandFlat;
using namespace std;


ParticleInjector::ParticleInjector() :
	fInputStream(0)
{	

	cout << "Running ParticleInjector() " << endl;

	Ro = 10*m;
	fFileName = "/home/alvaro/data/sims/sierra_grande/salida.shw";
	fInputStream = new ifstream(fFileName.c_str());
  	if (!fInputStream->is_open()) {
    ostringstream msg;
    G4cout << "Unable to open the requested file : " << fFileName;
    //throw IOFailureException("unable to open file");
  }

  ReadParticleFile();
}

void
ParticleInjector::ReadParticleFile()
{	

	G4cout << "[DEBUG] ParticleInjector: Running ReadParticleFile()" << G4endl;
	string line;
  unsigned int particleId;
  
	do {

    if (!getline(*fInputStream, line)) {
      fInputStream->close();
    }

    istringstream iss(line);
    
    if (line.find("#") == false){
    	cout << line << endl;
    	continue;
    }
  	
  	cout << line << endl;
    if (!(iss >> particleId
              >> px >> py >> pz
              >> x >> y
              >> z
              >> primaryShowerId
              >> primaryParticleId
              >> primaryEnergy
              >> primaryTheta
              >> primaryPhi)){
              //>> firstInteraction)) {
      cout << "Malformed line." << endl;
    }

  } while (particleId == 201 || particleId == 301);  // for G4's sake skip Deuterium or Tritium 
}

ParticleInjector::~ParticleInjector() 
{ }
