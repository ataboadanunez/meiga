#include "ConfigManager.h"
#include "G4UnitsTable.hh"

using boost::property_tree::ptree;
using namespace std;

void
ConfigManager::ReadDetectorList(string fDetectorList, Event& theEvent)
{
	cout << "ConfigManager::ReadDetectorList: Reading Detector Configuration from file " << fDetectorList << endl;
	Detector& detector = theEvent.GetDetector();
	int nModules = 0;

	// read XML with module positions
	ptree tree;
	read_xml(fDetectorList, tree);
	for (const auto& i : tree.get_child("moduleList")) {
		ptree subtree;
		string name;
		tie(name, subtree) = i;

		// do this for type cases
		if (name != "module")
			continue;

		vector<double> modulePosition;
		string moduleIdstr = subtree.get<string>("<xmlattr>.id");
		int moduleId = stoi(moduleIdstr);
		modulePosition.clear();

		// create and get module
		detector.MakeModule(moduleId);
		Module& module = detector.GetModule(moduleId);

		nModules+=1;

    std::cout << "[DEBUG] hola!" << std::endl;

		for (const auto &v : subtree.get_child("")) {
			string label = v.first;
			if ( label != "<xmlattr>" ) {
       
				if (label == "barsInPanel") {
					int nBarsInPanel = stoi(v.second.data());
					module.SetNBars(nBarsInPanel);
				}
				else if (label == "barLength") {
					double barLength = stoi(v.second.data());
					string unit = v.second.get<string>("<xmlattr>.unit");
					double unitG4 = G4UnitDefinition::GetValueOf(unit);
					module.SetBarLength(barLength*unitG4);
				}
				else {
					double value = stod(v.second.data());
					string unit = v.second.get<string>("<xmlattr>.unit");
					double valueG4 = value*G4UnitDefinition::GetValueOf(unit);
					modulePosition.push_back(value*G4UnitDefinition::GetValueOf(unit));
				}
			}
		}
     std::cout << "[DEBUG] adios! " << std::endl;
		module.SetModulePosition(modulePosition);
	}

}
