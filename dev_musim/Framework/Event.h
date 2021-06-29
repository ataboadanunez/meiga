#ifndef Event_h
#define Event_h 1

#include "Particle.h"
#include "SimData.h"
#include <vector>
#include <string>

/**
  MuSim Event Class

  This class is intended to store methods and variables
  that will be handled during the simulation / reconstruction
  processes. 

  todo: Description about "event" concept.

  author: Alvaro Taboada
  date: 16 June 2021

  $Id:$
*/


class Event 
{
  public:
    
    Event();
    virtual ~Event();
    // por ahora aqui. eventualmente sera un metodo de la clase SimData
    // y tomara como entrada el id del SiPM donde los fotones fueron detectados
    // teniendo en cuenta esto, quiza tenga mas sentido crear una clase SimDetectorData
    // y otra Detector...

    SimData& GetSimData() { return fSimData; }

  private:

    SimData fSimData;

};




#endif
