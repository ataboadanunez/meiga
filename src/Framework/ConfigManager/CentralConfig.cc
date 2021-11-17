#include "CentralConfig.h"
#include "G4UnitsTable.hh"

using boost::property_tree::ptree;
using namespace std;

void
CentralConfig::ReadDetectorList(string fDetectorList, Event& theEvent)
{
	cout << "CentralConfig::ReadDetectorList: Reading Detector Configuration from DetectorList " << fDetectorList << endl;
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

		for (const auto &v : subtree.get_child("")) {
			string label = v.first;
			if ( label != "<xmlattr>" ) {
				if (label != "barsInPanel") {
					double value = stod(v.second.data());
					string unit = v.second.get<string>("<xmlattr>.unit");
					double valueG4 = value*G4UnitDefinition::GetValueOf(unit);
					modulePosition.push_back(value*G4UnitDefinition::GetValueOf(unit));
					std::cout << "[DEBUG] ConfigManager::CentralConfig: Module Position (Geant4) = " << valueG4 << " " << unit << std::endl;
				}
				else {
					int nBarsInPanel = stoi(v.second.data());
					std::cout << "[DEBUG] ConfigManager::CentralConfig: Bars in panel = " << nBarsInPanel << std::endl;
					module.SetNBars(nBarsInPanel);
				}
			}
		}

		//detector.SetNModules(nModules);
		module.SetModulePosition(&modulePosition);
	}

}
