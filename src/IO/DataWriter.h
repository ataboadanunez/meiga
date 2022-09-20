#ifndef DataWriter_h_1
#define DataWriter_h_1


#include "Event.h"
#include "SimData.h"

#include <nlohmann/json.hpp>

class DataWriter {
	
	public:
		DataWriter();
		virtual ~DataWriter();

		static void FileWriter(Event& theEvent);

	private:
		static void SaveEnergy(nlohmann::json &jData, const SimData& sim, int id);
};

#endif