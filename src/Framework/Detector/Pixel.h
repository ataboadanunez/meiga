#ifndef Pixel_h
#define Pixel_h 1

//#include "SiPM.h"
#include <vector>

class Pixel
{
  public:
    Pixel() { }
    Pixel(const unsigned int id);
    virtual ~Pixel() { }

    // Generic Pixel stuff
    unsigned int GetId() { return fPixelId; }
    void SetId(unsigned int id) { fPixelId = id; }
    double GetPixelLength() { return fPixelLength; }
    double GetPixelWidth() { return fPixelWidth; }


    // SiPM stuff
    bool IsPhotonDetected(double energy);
    void SiPMSPEPulse(std::vector<double> &amplitude, double fBinTime, size_t fStartPulse);
    double GetSPEPulseDuration() { return fPulseDuration; }


  private:
    unsigned int fPixelId = 0;

    // this sould be placed in a config file
    double fPixelLength = 1.3;
    double fPixelWidth  = 0.1;

    // sipm SPE pulse parameters
    double fPulseDuration = 100.;
    double fA0 = 0.013; // mV
    double fT0 = -1.160; // ns
    double fTr = 2.679; // ns
    double fTf = 0.329; // ns
  
};

#endif