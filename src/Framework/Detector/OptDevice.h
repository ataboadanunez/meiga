#ifndef OptDevice_h
#define OptDevice_h 1

//#include "SiPM.h"
#include <vector>
#include <string>

class OptDevice
{
	public:
		enum DeviceType {
			eSiPM,
			ePMT
		};

		OptDevice() { }
		OptDevice(unsigned int id, OptDevice::DeviceType type);
		virtual ~OptDevice() { }

		
		// Generic OptDevice stuff
		unsigned int GetId() { return fOptDeviceId; }
		void SetId(unsigned int id) { fOptDeviceId = id; }

		double GetLength() { return fOptDeviceLength; }
		void SetLength(double len) { fOptDeviceLength = len; }

		double GetWidth() { return fOptDeviceWidth; }
		void SetWidth(double wid) { fOptDeviceWidth = wid; }
		
		DeviceType GetType() { return fType; }
		const std::string GetName();

		// response stuff
		bool IsPhotonDetected(double energy);
		void SPEPulse(std::vector<double> &amplitude, double fBinTime, size_t fStartPulse);
		double GetSPEPulseDuration() { return fPulseDuration; }


	private:
		unsigned int fOptDeviceId = 0;
		DeviceType fType; 

		// this sould be placed in a config file
		double fOptDeviceLength = 1.3;
		double fOptDeviceWidth  = 0.1;

		// sipm SPE pulse parameters
		double fPulseDuration = 100.;
		double fA0 = 0.013; // mV
		double fT0 = -1.160; // ns
		double fTr = 2.679; // ns
		double fTf = 0.329; // ns
	
};

#endif