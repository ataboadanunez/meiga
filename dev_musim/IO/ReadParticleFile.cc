// Implementation of ReadParticleFile class
#include "ReadParticleFile.h"
#include "CorsikaUtilities.h"
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

Event
ReadParticleFile::EventFileReader(const std::string &fileName)
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
  
  cout << "[INFO] Event::ReadParticleFile: Reading Input File " << fileName << endl;
  string errmsg;
  string info;

  ifstream fInputStream(fileName);
  Event ret;
  SimData& simData = ret.GetSimData();

  unsigned int particleId;
  unsigned int NumberOfParticles = 0;
  
  do {
    
    double px;
    double py;
    double pz;
    double x;
    double y;
    double time;
    unsigned primaryShowerId;
    unsigned primaryParticleId;
    double   primaryEnergy;
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
    //std::cout << line << std::endl;
    if (!(iss >> particleId
              >> px >> py >> pz
              >> x >> y
              >> time
              >> primaryShowerId
              >> primaryParticleId
              >> primaryEnergy
              >> primaryTheta
              >> primaryPhi)){
      
      cerr << "ReadParticleFile: Malformed line !!!" << endl;
      break;
    }

    vector<double> particlePosition{x, y, time};
    vector<double> particleMomentum{px, py, pz};
    //Particle particle(particleId, px, py, pz, x, y, time); 
    Particle particle(particleId, particlePosition, particleMomentum);
    simData.InsertParticle(particle);

    NumberOfParticles++;
  } while (true);


  simData.SetTotalNumberOfParticles(NumberOfParticles);
  //std::cout << "[INFO] Event::ReadParticleFile: Total number of particles in file = " << NumberOfParticles << std::endl;
  return ret;
  


}