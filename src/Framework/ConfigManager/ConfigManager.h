#ifndef ConfigManager_h
#define ConfigManager_h 1
// Meiga headers
#include "Event.h"
#include "SimData.h"
#include "Detector.h"

// Geant4 headers
#include "G4UnitsTable.hh"

// c++ headers
#include <map>
#include <string>
#include <string>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string/trim.hpp>

using boost::property_tree::ptree;
using namespace std;

class ConfigManager
{
	// Manager of configuration files

	public:

		ConfigManager() { }
		~ConfigManager() { }
		// public and static to be called within the applications 'without object'
		static void ReadConfigurationFile(Event& aEvent, const std::string &filename);
		static void ReadDetectorList(const std::string &filename, Event& theEvent);
		static void PrintConfig(const Event::Config &cfg);

		static void SetupParticleInection(const ptree &tree, SimData &aSimData);

		// return value giving the XML path
		template <typename T>
		inline static T GetPropertyFromXML(const ptree &tree, const string &branchName, const string &property, bool hasUnit = true) {

			T res = tree.get<T>(branchName+"."+property);
			
			if (hasUnit) {
				auto unit = G4UnitDefinition::GetValueOf(tree.get<string>(branchName+"."+property+".<xmlattr>.unit"));
				res *= unit;
			} 
			
			return res;
		}

		template <typename T>
		inline static T GetPropertyFromXML(const ptree &tree, const string &branchName, const string &property, const T &defaultVal, bool hasUnit = true, const string &defaultUnit = "mm") {
			
			T res = tree.get<T>(branchName+"."+property, defaultVal);
			
			if (hasUnit) {
				auto unit = G4UnitDefinition::GetValueOf(tree.get<string>(branchName+"."+property+".<xmlattr>.unit", defaultUnit));
				res *= unit;
			} 
			
			return res;
		}
};

#endif
