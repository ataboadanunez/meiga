#include "ConfigManager.h"
#include "G4UnitsTable.hh"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <map>
#include <string>

using boost::property_tree::ptree;
using namespace std;

void
ConfigManager::ReadDetectorList(string fDetectorList, Event& theEvent)
{

	cout << "[INFO] ConfigManager::ReadDetectorList: Reading Detector Configuration from file " << fDetectorList << endl;
	
	int nDetectors = 0;
	// used to determine maximum value of Z coordinate of current detectors in file
	double maxHeight = 0.;
	
	// read XML with detector positions
	ptree tree;
	read_xml(fDetectorList, tree);
	for (const auto& i : tree.get_child("detectorList")) {
		ptree subtree;
		string name;
		tie(name, subtree) = i;

		if (name != "detector")
			continue;

		vector<double> detPosition;
		string detIdstr = subtree.get<string>("<xmlattr>.id");
		string detTypestr = subtree.get<string>("<xmlattr>.type");
		int detId = stoi(detIdstr);
		Detector::DetectorType detType = theEvent.GetDetector().StringToType(detTypestr);
		detPosition.clear();

		cout << "[DEBUG] ConfigManager::ReadDetectorList: Reading configuration of detector " << detTypestr << " with ID = " << detId << endl;
		// register detector in the Event
		if (!theEvent.HasDetector(detId)) {
			theEvent.MakeDetector(detId, detType);
		}
		else {
			cout << "[WARNING] ConfigManager::ReadDetectorList: A Detector " << " with ID = " << detId << " already registered! IDs must be unique, please check your DetectorList.xml!" << endl;
			continue;
		}
		
		Detector& detector = theEvent.GetDetector(detId);
		// search for detector properties in the DetectorList.xml
		detector.SetDetectorProperties(detType, fDetectorList);

		nDetectors+=1;

		for (const auto &v : subtree.get_child("")) {
			string label = v.first;
			if ( label != "<xmlattr>" ) {
				string value = v.second.data();
				boost::algorithm::trim(value);
				cout << "[DEBUG] ConfigManager::ReadDetectorList: Label = " << label << endl;
				double dValue = stod(value);
				string unit = v.second.get<string>("<xmlattr>.unit");
				double coord = G4UnitDefinition::GetValueOf(unit) * dValue;
				detPosition.push_back(coord);

				if ((label == "z") && (coord > maxHeight))
					maxHeight = coord;
				
			}
		}
		
		cout << "[DEBUG] ConfigManager::ReadDetectorList: Found maximum height of detectors = " << maxHeight / CLHEP::cm << " cm " << endl;
		theEvent.SetMaximumHeight(maxHeight);
		detector.SetDetectorPosition(detPosition);
	}

}
