// Meiga headers
#include "ConfigManager.h"
#include "ReadParticleFile.h"
// Geant4 headers
#include "G4UnitsTable.hh"

using boost::property_tree::ptree;
using namespace std;

/** Static class member definition **/
DefaultProperties ConfigManager::defProp;

Event
ConfigManager::ReadConfigurationFile(const string &fConfigFile)
{
	
	Event theEvent;
	Event::Config &cfg = theEvent.GetConfig();
	ptree tree;
	read_json(fConfigFile, tree);

	// reads the input file and fills the Event with particles
	string fInputFileName = tree.get<string>("InputFile");
	ReadParticleFile::ParticleFileReader(fInputFileName, theEvent);

	SimData& simData = theEvent.GetSimData();

	cfg.fInputFileName = tree.get<string>("InputFile");
	cfg.fDetectorList  = tree.get<string>("DetectorList");
	cfg.fDetectorProperties = tree.get<string>("DetectorProperties");
	defProp.SetDefaultProperties(cfg.fDetectorProperties);

	cfg.fSimulationMode = tree.get<string>("Simulation.SimulationMode");
	simData.SetSimulationMode(simData.ModeConversion(cfg.fSimulationMode));
	cfg.fInjectionMode  = tree.get<string>("Simulation.InjectionMode");
	simData.SetInjectionMode(simData.InjectionConversion(cfg.fInjectionMode));
	cfg.fGeoVis  = tree.get<bool>("Simulation.GeoVisOn");
	cfg.fTrajVis = tree.get<bool>("Simulation.TrajVisOn");
	cfg.fCheckOverlaps = tree.get<bool>("Simulation.CheckOverlaps");
	cfg.fRenderFile = tree.get<string>("Simulation.RenderFile");
	cfg.fPhysicsListName = tree.get<string>("Simulation.PhysicsName");
	cfg.fVerbosity = tree.get<int>("Simulation.Verbosity");

	cfg.fOutputFileName = tree.get<string>("Output.OutputFile");
	cfg.fCompressOutput = tree.get<bool>("Output.CompressOutput");
	cfg.fSavePETimeDistribution = tree.get<bool>("Output.SavePETimeDistribution");
	cfg.fSaveComponentsPETimeDistribution = tree.get<bool>("Output.SaveComponentsPETimeDistribution");
	cfg.fSaveTraces     = tree.get<bool>("Output.SaveTraces");
	cfg.fSaveEnergy     = tree.get<bool>("Output.SaveEnergy");
	cfg.fSaveComponentsEnergy = tree.get<bool>("Output.SaveComponentsEnergy");

	return theEvent;

}

void
ConfigManager::ReadDetectorList(const string &fDetectorList, Event& theEvent)
{
	
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

		cout << "[INFO] ConfigManager::ReadDetectorList: Reading configuration of detector " << detTypestr << " with ID = " << detId << endl;
		// register detector in the Event
		if (!theEvent.HasDetector(detId)) {
			theEvent.MakeDetector(detId, detType);
		}
		else {
			cout << "[WARNING] ConfigManager::ReadDetectorList: A Detector " << " with ID = " << detId << " already registered! IDs must be unique, please check your DetectorList.xml!" << endl;
			continue;
		}
		
		Detector& detector = theEvent.GetDetector(detId);
		SimData& simData = theEvent.GetSimData();
		string fPropertiesFile = simData.GetDetectorPropertiesFile();
		
		// set detector position
		for (const auto &v : subtree.get_child("")) {
			string label = v.first;

			if ( label != "<xmlattr>" ) {
				if ((label == "x") || (label == "y") || (label == "z")) {
					string value = v.second.data();
					boost::algorithm::trim(value);
					double dValue = stod(value);
					string unit = v.second.get<string>("<xmlattr>.unit");
					double coord = G4UnitDefinition::GetValueOf(unit) * dValue;
					detPosition.push_back(coord);
					// compute maximum height according to Z coordinate
					if ((label == "z") && (coord > maxHeight))
						maxHeight = coord;	
				}
				
				
			}
		}
		
		// shouldn't be part of DetectorSimData?
		theEvent.SetMaximumHeight(maxHeight);
		detector.SetDetectorPosition(detPosition);

		// search for another detector properties in the DetectorList.xml
		detector.SetDetectorProperties(subtree, defProp);
	}

}

void
ConfigManager::PrintConfig(const Event::Config &cfg)
{
	cout << "[INFO] ConfigManager::PrintConfig:" << endl;
	cout << "[INFO] Using the following configuration:" << endl;
	cout << "[INFO] InputFile = " << cfg.fInputFileName << endl;
	cout << "[INFO] OutputFile = " << cfg.fOutputFileName << endl;
	cout << "[INFO] Save PE Time distribution (components) = " << (cfg.fSavePETimeDistribution ? "yes" : "no") << " (" << (cfg.fSaveComponentsPETimeDistribution ? "yes)" : "no)") << endl;
	cout << "[INFO] Compress Output = " << (cfg.fCompressOutput ? "yes" : "no") << endl;
	cout << "[INFO] Save Traces = " << (cfg.fSaveTraces ? "yes" : "no") << endl;
	cout << "[INFO] Save Energy Deposit (components) = " << (cfg.fSaveEnergy ? "yes" : "no") << " (" << (cfg.fSaveComponentsEnergy ? "yes)" : "no)") << endl;
	cout << "[INFO] DetectorList = " << cfg.fDetectorList << endl;
	cout << "[INFO] DetectorProperties = " << cfg.fDetectorProperties << endl;
	cout << "[INFO] SimulationMode = " << cfg.fSimulationMode << endl;
	cout << "[INFO] InjectionMode = " << cfg.fInjectionMode << endl;
	cout << "[INFO] VisualizeGeometry = " << (cfg.fGeoVis ? "yes" : "no") << endl;
	cout << "[INFO] VisualizeTrajectory = " << (cfg.fTrajVis ? "yes" : "no") << endl;
	cout << "[INFO] RenderFile = " << cfg.fRenderFile << endl;
	cout << "[INFO] PhysicsList = " << cfg.fPhysicsListName << endl;
	cout << "[INFO] CheckOverlaps = " << (cfg.fCheckOverlaps ? "yes" : "no") << endl;

}