// Implementation of ReadParticleFile class
#include "ReadParticleFile.h"
#include "CorsikaUtilities.h"
#include "PhysicsConstants.h"
#include "Particle.h"
#include "SimData.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>


using namespace std;

ReadParticleFile::ReadParticleFile() 
{

}

ReadParticleFile::~ReadParticleFile()
{

}

void
ReadParticleFile::ParticleFileReader(const std::string &fileName, Event& theEvent)
{
  /*
  This function gets as input a file with particle information.
  This file must have the structure N_rows x 12_columns
  where each column corresponds to the following information:

  particle ID, particle momentum (px, py, pz), particle position (x, y, time), 
  ID the shower primary, ID of the primary particle, energy of primary, 
  zenith, azimuth of primary (last three quantities correspond to shower primary).


  IDs and coordinates are given using CORSIKA convention. Therefore, a proper conversion of
  units must be performed (e.g. to Geant4 units).


  */
  
  cout << "[INFO] ReadParticleFile::ParticleFileReader: Reading Input File " << fileName << endl;
  string errmsg;
  string info;

  ifstream fInputStream(fileName);
  //Event ret;
  SimData& simData = theEvent.GetSimData();

  unsigned int particleId;
  unsigned int NumberOfParticles = 0;
  
  do {
    
    double px;
    double py;
    double pz;
    double x;
    double y;
    double z;
    unsigned int primaryShowerId;
    unsigned int primaryParticleId;
    double primaryEnergy;
    double primaryTheta;
    double primaryPhi;
    
    std::string line;
    
    if (!fInputStream) {
      errmsg = "[ERROR] Event::ReadParticleFile: Error(s) occurred while reading input file!\n Possible causes of this fail:\n - Misspelled file name\n - Wrong file location\n - Wrong file format ";
      cerr << errmsg << endl;
    }
    
    if (!getline(fInputStream, line)) {
      fInputStream.close();
      break;
    }


    std::istringstream iss(line);  

    if (line[0] == '#')
    	continue;

    if (!(iss >> particleId
              >> px >> py >> pz
              >> x >> y
              >> z
              >> primaryShowerId
              >> primaryParticleId
              >> primaryEnergy
              >> primaryTheta
              >> primaryPhi)){
      
      cerr << "[ERROR] Event::ReadParticleFile: Malformed line !!!" << endl;
      break;
    }

    /// note on conversion units
    /*
			units should be fix here. we use directly
			G4 system of units to avoid re-conversions

			input is given in CORSIKA units, i.e.,
			[x] = cm
			[p, E] = GeV
			[theta, phi] = deg
			
			particle's IDs are CORSIKA ids.
			we use the CORSIKA id and then 
			convert it using Framework functions
			to PDG encoding (for Geant4)

    */
    
    x*=cm;
    y*=cm;
    px*=GeV;
    py*=GeV;
    pz*=GeV;
    z*=cm;
    primaryEnergy*=GeV;
    primaryTheta*=deg;
    primaryPhi*=deg;
		
    vector<double> particlePosition{x, y, z};
    vector<double> particleMomentum{px, py, pz};
    Particle particle(particleId, particlePosition, particleMomentum);
    simData.InsertParticle(particle);

    NumberOfParticles++;
  } while (true);


  simData.SetTotalNumberOfParticles(NumberOfParticles);
  //std::cout << "[INFO] Event::ReadParticleFile: Total number of particles in file = " << NumberOfParticles << std::endl;
  //return ret;
  


}
