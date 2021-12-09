// definition of the ConfigParser class

#ifndef CentralConfig_h
#define CentralConfig_h 1

#include "Event.h"
#include "Detector.h"
#include "Module.h"

#include <string>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>


class CentralConfig
{
	// description

	public:

		CentralConfig() { }
		~CentralConfig() { }

		static void ReadDetectorList(std::string filename, Event& theEvent);


};


#endif