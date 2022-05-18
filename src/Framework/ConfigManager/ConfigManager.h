#ifndef ConfigManager_h
#define ConfigManager_h 1

// Meiga headers
#include "Event.h"
#include "SimData.h"
#include "Detector.h"

// c++ headers
#include <map>
#include <string>
#include <string>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string/trim.hpp>



class ConfigManager
{
	// Manager of configuration files

	public:

		ConfigManager() { }
		~ConfigManager() { }

		static Event ReadConfiguration(std::string filename);
		static void ReadDetectorList(std::string filename, Event& theEvent);


};


#endif