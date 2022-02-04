#include "ConfigManager.h"
#include "G4UnitsTable.hh"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <map>
#include <string>

using boost::property_tree::ptree;
using namespace std;

static map<string, enum Detector::DetectorType> conversion;

static void initConversionMap() {
	static bool isInitialized = false;
	if (isInitialized) return;
	isInitialized = true;
	conversion.clear();
	conversion["eUnknown"] = Detector::eUnknown;
	conversion["eMusaic"]  = Detector::eMusaic;
	conversion["eMudulus"] = Detector::eMudulus;
}

void
ConfigManager::ReadDetectorList(string fDetectorList, Event& theEvent)
{
	initConversionMap();
	cout << "ConfigManager::ReadDetectorList: Reading Detector Configuration from file " << fDetectorList << endl;
	//Detector& detector = theEvent.GetDetector();
	int nDetectors = 0;

	// read XML with detector positions
	ptree tree;
	read_xml(fDetectorList, tree);
	for (const auto& i : tree.get_child("detectorList")) {
		ptree subtree;
		string name;
		tie(name, subtree) = i;
#warning "add detector cases"
		// do this for type cases
		if (name != "detector")
			continue;

		vector<double> detPosition;
		string detIdstr = subtree.get<string>("<xmlattr>.id");
		int detId = stoi(detIdstr);
		detPosition.clear();

		// create and get detector
		if (!theEvent.HasDetector(detId)) {
			theEvent.MakeDetector(detId);
		}
		
		Detector& detector = theEvent.GetDetector(detId);

		nDetectors+=1;

		for (const auto &v : subtree.get_child("")) {
			string label = v.first;
			if ( label != "<xmlattr>" ) {
				string value = v.second.data();
				boost::algorithm::trim(value);

				if (label == "type") {
					std::cout << " reading detector type " << std::endl;
					try {
						cout << value << endl;
						auto detType = conversion[value];
						detector.SetType(detType);
					}
					catch (out_of_range &e) {
						cerr << "Detector type " << value << " does not exist!" << endl;
						detector.SetType(Detector::eUnknown);
					}
				}	
				else if (label == "barsInPanel") {
					int nBarsInPanel = stoi(value);
					detector.SetNBars(nBarsInPanel);
				}
				else if (label == "barLength") {
					cout << value << endl;
					double barLength = stod(value);
					string unit = v.second.get<string>("<xmlattr>.unit");
					double unitG4 = G4UnitDefinition::GetValueOf(unit);
					detector.SetBarLength(barLength*unitG4);
				}
				else {
					cout << value << endl;
					double dValue = stod(value);
					string unit = v.second.get<string>("<xmlattr>.unit");
					detPosition.push_back(dValue*G4UnitDefinition::GetValueOf(unit));
				}
			}
		}
		
		detector.SetDetectorPosition(detPosition);
	}

}
