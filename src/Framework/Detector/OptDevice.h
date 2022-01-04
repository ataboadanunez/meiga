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
		OptDevice(int id, OptDevice::DeviceType type);
		OptDevice(OptDevice::DeviceType type);
		virtual ~OptDevice() { }

		
		// Generic OptDevice stuff
		int GetId() { return fOptDeviceId; }
		void SetId(int id) { fOptDeviceId = id; }

		DeviceType GetType() { return fType; }
		void SetType(OptDevice::DeviceType type) { fType = type; }
		const std::string GetName();

		double GetLength() { return fOptDeviceLength; }
		void SetLength(double len) { fOptDeviceLength = len; }

		double GetWidth() { return fOptDeviceWidth; }
		void SetWidth(double wid) { fOptDeviceWidth = wid; }

		double GetThickness() { return fOptDeviceThickness; }
		void SetThickness(double thi) { fOptDeviceThickness = thi; }

		

		// response stuff
		bool IsPhotonDetected(double energy);
		void SPEPulse(std::vector<double> &amplitude, double fBinTime, size_t fStartPulse);
		double GetSPEPulseDuration() { return fPulseDuration; }



	private:
		int fOptDeviceId = 0;
		DeviceType fType; 

		// this sould be placed in a config file
		double fOptDeviceLength = 0.0;
		double fOptDeviceWidth  = 0.0;
		double fOptDeviceThickness = 0.0;

		// SiPM properties
		double fSiPMLength 		= 1.3;
		double fSiPMWidth  		= 1.3;
		double fSiPMThickness = 0.1;
		// add quantum efficiency

		// PMT properties
		double fPMTLength 		= 1.3;
		double fPMTWidth  		= 1.3;
		double fPMTThickness = 0.1;
		// add quantum efficiency


		// sipm SPE pulse parameters
		double fPulseDuration = 100.;
		double fA0 = 0.013; // mV
		double fT0 = -1.160; // ns
		double fTr = 2.679; // ns
		double fTf = 0.329; // ns
	
};

#endif