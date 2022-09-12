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

#warning "move to DetectorSimData!"
		void SetMaximumHeight(double maxH) { fMaximumHeight = maxH; }
		double GetMaximumHeight() { return fMaximumHeight; }
		
		std::map<int, Detector>& DetectorRange() { return fDetectorMap; }
		const std::map<int, Detector>& DetectorRange() const { return fDetectorMap; }

		struct Config {
	
		// program configuration variables

		// input variables
		std::string fInputFileName;
		std::string fDetectorList;
		std::string fDetectorProperties;

		// simulation variables
		std::string fSimulationMode;
		std::string fInjectionMode;
		bool fGeoVis;
		bool fTrajVis;
		bool fCheckOverlaps;
		std::string fRenderFile;
		std::string fPhysicsListName;
		int fVerbosity;

		// output variables
		std::string fOutputFileName;
		bool fCompressOutput;
		bool fSaveTraces;
		bool fSaveEnergy;

		// add more stuff below in case is needed


		};

		Config& GetConfig() { return cfg; }
		const Config& GetConfig() const { return cfg; }
		void SetConfig(const Event::Config &ncfg) { cfg = ncfg; }

	private:

		Config cfg;
		SimData fSimData;
		Detector fDetector;

		std::map<int, Detector> fDetectorMap;
		int fNDetectors = 0;
		double fMaximumHeight;


};




#endif
