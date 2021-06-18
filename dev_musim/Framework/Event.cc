// implementation of Event class
#include "Event.h"
#include "Particle.h"
#include <iostream>
#include <fstream>
#include <sstream>

// esto aqui no pinta nada. escribir otra clase EventFileReader que se llame desde el main()
Event 
Event::ReadParticleFile(const std::string &fileName)
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
  std::cout << "[INFO] Event::ReadParticleFile: Reading Input File " << fileName << std::endl;

  std::string errmsg;
  std::string info;

  std::ifstream fInputStream(fileName);
  Event ret;

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
      std::cerr << errmsg << std::endl;
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
      
      std::cerr << "ReadParticleFile: Malformed line !!!" << std::endl;
      break;
    }

    Particle particle(particleId, px, py, pz, x, y, time); 
    ret.InsertParticle(particle);

    NumberOfParticles++;
  } while (true);


  ret.SetTotalNumberOfParticles(NumberOfParticles);
  std::cout << "[INFO] Event::ReadParticleFile: Total number of particles in file = " << NumberOfParticles << std::endl;
  return ret;

}
