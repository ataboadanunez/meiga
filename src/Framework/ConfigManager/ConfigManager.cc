#include "ConfigManager.h"
#include "G4UnitsTable.hh"

using boost::property_tree::ptree;
using namespace std;

void
ConfigManager::ReadDetectorList(string fDetectorList, Event& theEvent)
{
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
		if (!theEvent.HasDetector(detId))
			theEvent.MakeDetector(detId);
		
		Detector& detector = theEvent.GetDetector(detId);

		nDetectors+=1;

		for (const auto &v : subtree.get_child("")) {
			string label = v.first;
			if ( label != "<xmlattr>" ) {

				if (label == "type") {
					std::cout << " reading detector type " << std::endl;
					auto detType = (Detector::DetectorType)stoi(v.second.data());
					detector.SetType(detType);
				}	
				else if (label == "barsInPanel") {
					int nBarsInPanel = stoi(v.second.data());
					detector.SetNBars(nBarsInPanel);
				}
				else if (label == "barLength") {
					double barLength = stoi(v.second.data());
					string unit = v.second.get<string>("<xmlattr>.unit");
					double unitG4 = G4UnitDefinition::GetValueOf(unit);
					detector.SetBarLength(barLength*unitG4);
				}
				else {
					double value = stod(v.second.data());
					string unit = v.second.get<string>("<xmlattr>.unit");
					detPosition.push_back(value*G4UnitDefinition::GetValueOf(unit));
				}
			}
		}
		
		detector.SetDetectorPosition(detPosition);
	}

}
