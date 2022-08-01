#ifndef ConfigManager_h
#define ConfigManager_h 1

// Meiga headers
#include "Event.h"
#include "SimData.h"
#include "Detector.h"
#include "DefaultProperties.h"

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

		ConfigManager() {
			//TODO: Mapa que entre por detectorType y entregue una clase con las propiedades por defecto
		}
		~ConfigManager() { }

		static Event ReadConfiguration(const std::string &filename);
		static void ReadDetectorList(const std::string &filename, Event& theEvent);

	private:
		// static void ActuallyReadDetectorXML(std::string filename, Event& theEvent);
		static DefaultProperties defProp;
};

#endif
