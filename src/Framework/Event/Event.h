#ifndef Event_h
#define Event_h 1

#include "Particle.h"
#include "SimData.h"
#include "Detector.h"
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

    // interface to fwk/SimData
    SimData& GetSimData() { return fSimData; }

    // interface to fwk/Detector
    Detector& GetDetector() { return fDetector; }
    Detector& GetDetector(unsigned int id) { return fDetectorMap[id]; }
    bool HasDetector(unsigned int id);
    void MakeDetector(unsigned int id);
    void MakeDetector(unsigned int id, Detector::DetectorType type);

    int GetNDetectors() {return fNDetectors; }

    std::map<int, Detector>& DetectorRange() { return fDetectorMap; }
    const std::map<int, Detector>& DetectorRange() const { return fDetectorMap; }

  private:

    SimData fSimData;
    Detector fDetector;

    std::map<int, Detector> fDetectorMap;
    int fNDetectors = 0;
};




#endif
