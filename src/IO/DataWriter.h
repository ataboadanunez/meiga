#ifndef DataWriter_h_1
#define DataWriter_h_1


#include "Event.h"
#include "SimData.h"
#include "DetectorSimData.h"

#include <nlohmann/json.hpp>

class DataWriter {
	
	public:
		DataWriter();
		virtual ~DataWriter();

		static void FileWriter(Event& theEvent);

	private:
		
		static void SaveInputFlux(nlohmann::json &jData, SimData &simData);

		static void SavePETimeDistribution(nlohmann::json &jData, const DetectorSimData& sim, int detId, int odId, bool saveComponents);

		static void SaveCharge(nlohmann::json &jData, const DetectorSimData &sim, int detId, int odId);

		static void SaveTraces(nlohmann::json &jData, const DetectorSimData& sim, int detId, int odId);

		static void SaveEnergy(nlohmann::json &jData, const SimData& sim, int id, const bool saveComponents);

		// for scintillator bar counters
		static void SaveBinaryCounter(nlohmann::json &jData, const DetectorSimData& sim, int id);
};

#endif