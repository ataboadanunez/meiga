#ifndef ConfigManager_h
#define ConfigManager_h 1

#include "Event.h"
#include "Detector.h"
#include "Module.h"

#include <string>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>


class ConfigManager
{
	// Manager of configuration files

	public:

		ConfigManager() { }
		~ConfigManager() { }

		static void ReadDetectorList(std::string filename, Event& theEvent);


};


#endif