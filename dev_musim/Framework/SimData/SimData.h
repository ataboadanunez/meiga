#ifndef SimData_h
#define SimData_h

#include "Particle.h"
#include "DetectorSimData.h"
#include <vector>
#include <map>


class SimData
{
  public:
    SimData();
    ~SimData();

    typedef std::vector<Particle> ParticleVector;
    const ParticleVector& GetParticleVector() const { return fParticles; }
    void InsertParticle(const Particle& particle) { fParticles.push_back(particle); }

    unsigned int GetTotalNumberOfParticles() const { return fTotalNumberOfParticles; }
    void SetTotalNumberOfParticles(const unsigned int n) { fTotalNumberOfParticles = n; }

    void AddPhotonTime(const double peTime);
    std::vector<double> GetPhotonTime() { return fPETimes; }

    // add DetectorSimData getters
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
    unsigned int fTotalNumberOfParticles = 0;
    std::vector<double> fPETimes;

    DetectorSimData fDetectorSimData;
    std::map<int, DetectorSimData> fDetectorSimDataMap;
    //SiPMSimData fSiPMSimData;
    // precio que hay que pagar por no usar punteros? 
    //std::map<int, SiPMSimData> fSiPMSimMap;

};

#endif
