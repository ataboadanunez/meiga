#ifndef DataWriter_h_1
#define DataWriter_h_1


#include "Event.h"
#include "SimData.h"
#include "DetectorSimData.h"
#include "OptDeviceSimData.h"

// unable warning of shadow declarations when including json nlohmann
// variable `s` is shadow declared with Geant4 unit `second`
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "json.hpp"
#pragma GCC diagnostic pop

class DataWriter {
	
	public:
		DataWriter();
		virtual ~DataWriter();

		static void FileWriter(Event& theEvent);

	private:
		
		static void SaveInputFlux(nlohmann::ordered_json &jEvent, SimData &simData, size_t eventId);

		static void SavePETimeDistribution(nlohmann::ordered_json &jData, const OptDeviceSimData& sim, int detId, int odId, size_t eventId, const bool, const Particle::Component comp);

		static void SaveCharge(nlohmann::ordered_json &jEvent, const OptDeviceSimData & sim, int detId, int odId, size_t eventId);

		static void SaveDepositedEnergy(nlohmann::ordered_json &jEvent, const DetectorSimData& sim, int detId, size_t eventId);

		// for scintillator bar counters
		static void SaveBinaryCounter(nlohmann::ordered_json& jEvent, const DetectorSimData& sim, int detId, size_t eventId);
};

#endif