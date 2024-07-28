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

		SimData& GetSimData() { return fSimData; }

		Detector& GetDetector() { return fDetector; }
		Detector& GetDetector(unsigned int id) { return fDetectorMap[id]; }
		bool HasDetector(unsigned int id);
		void MakeDetector(unsigned int id, Detector::DetectorType type);
		void MakeDetector(const Detector & aDetector);

		std::map<int, Detector>& DetectorRange() { return fDetectorMap; }
		const std::map<int, Detector>& DetectorRange() const { return fDetectorMap; }
		int GetNDetectors() {return fNDetectors; }
		std::vector<int> GetDetectorIds();

		// should be part of DetectorSimData
		void SetMaximumHeight(double maxH) { fMaximumHeight = maxH; }
		double GetMaximumHeight() { return fMaximumHeight; }

		struct Config {
	
			// program configuration variables
			std::string fConfigurationFileName;
			// input variables
			std::string fInputMode;
			unsigned int fInputNParticles;
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
			bool fSaveInput;
			bool fSavePETimeDistribution;
			bool fSaveComponentsPETimeDistribution;
			bool fSaveTraces;
			bool fSaveEnergy;
			bool fSaveComponentsEnergy;
			bool fSaveCharge;
			bool fSaveCounts;
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
