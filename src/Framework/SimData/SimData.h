#ifndef SimData_h
#define SimData_h

#include "Particle.h"
#include "DetectorSimData.h"
#include <string>
#include <vector>
#include <map>


class SimData
{
  public:
    SimData();
    ~SimData();

    typedef std::vector<Particle> ParticleVector;
    const ParticleVector& GetParticleVector() const { return fParticles; }
    ParticleVector& GetParticleVector()  { return fParticles; }
    void InsertParticle(Particle& particle) { fParticles.push_back(particle); }

    unsigned int GetTotalNumberOfParticles() const { return fTotalNumberOfParticles; }
    void SetTotalNumberOfParticles(const unsigned int n) { fTotalNumberOfParticles = n; }

    Particle& GetCurrentParticle() { return fCurrentParticle; }
    void SetCurrentParticle(Particle particle) { fCurrentParticle = particle; }

    void SetSimulationMode(const std::string simMode) { fSimulationMode = simMode; }
    const std::string GetSimulationMode() const;

    // DetectorSimData getters
    void MakeDetectorSimData(unsigned int id);
    DetectorSimData& GetDetectorSimData() { return fDetectorSimData; }
    DetectorSimData& GetDetectorSimData(unsigned int id) { return fDetectorSimDataMap[id]; }
    
    /***
    void MakeSiPMSimData(unsigned int id);
    SiPMSimData& GetSiPMSimData() { return fSiPMSimData; }
    
    // get SiPMSimData passing SiPM id
    // eventually create SiPM class and GetSiPMSimData as a member of SiPM
    // here would be GetSiPM(unsigned int id)... blablabla 
    SiPMSimData& GetSiPMSimData(unsigned int id) { return fSiPMSimMap[id]; }
    */
    

  private:

    ParticleVector fParticles;
    Particle fCurrentParticle;
    unsigned int fTotalNumberOfParticles = 0;
    std::string fSimulationMode;

    DetectorSimData fDetectorSimData;
    std::map<int, DetectorSimData> fDetectorSimDataMap;
    //SiPMSimData fSiPMSimData;
    // precio que hay que pagar por no usar punteros? 
    //std::map<int, SiPMSimData> fSiPMSimMap;

};

#endif
