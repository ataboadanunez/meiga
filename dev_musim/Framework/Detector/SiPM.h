#ifndef SiPM_h
#define SiPM_h 1

#include <vector>


class SiPM {

	// SiPM detector class
	// author: alvaro taboada


	public:
		SiPM() { }
		SiPM(const unsigned int id);
		virtual ~SiPM() { }

		unsigned int GetId() { return fSiPMId; }

		double GetSiPMLength() { return fSiPMLength; }
		double GetSiPMWidth()  { return fSiPMWidth; }

		bool IsPhotonDetected(double energy);

		void SPEPulse(std::vector<double> &amplitude, double fBinTime, size_t fStartPulse);
		double GetSPEPulseDuration() { return fPulseDuration; }


	private:

		// this sould be placed in a config file
		double fSiPMLength = 1.3;
    double fSiPMWidth  = 0.1;
    unsigned int fSiPMId = 0;

    // sipm SPE pulse parameters
    double fPulseDuration = 100.;


};


#endif