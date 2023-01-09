// Meiga headers
#include "ConfigManager.h"
#include "ParticleFiller.h"
// Geant4 headers
#include "G4UnitsTable.hh"

using boost::property_tree::ptree;
using namespace std;

/** Static class member definition **/
DefaultProperties ConfigManager::defProp;

Event
ConfigManager::ReadConfigurationFile(const string &fConfigFile)
{
	
	cout << "[INFO] ConfigManager::ReadConfigurationFile: Reading configuration File " << fConfigFile << endl;
	Event theEvent;
	Event::Config &cfg = theEvent.GetConfig();
	ptree tree;
	read_json(fConfigFile, tree);
	

	SimData& simData = theEvent.GetSimData();
	// input
	cfg.fInputMode = tree.get<string>("Input.Mode");
	simData.SetInputMode(simData.InputModeConversion(cfg.fInputMode));
	cfg.fInputFileName = tree.get<string>("Input.InputFileName");
	cfg.fInputNParticles = tree.get<unsigned int>("Input.InputNParticles");

	// reads the input (ARTI) file and fills the Event with particles
	if (simData.GetInputMode() == SimData::InputMode::eUseARTI) {
		cout << "[WARNING] ConfigManager::ReadConfigurationFile: Selected InputMode = eUseARTI. An input file is needed." << endl;
		ParticleFiller::FillParticleVector(cfg.fInputFileName, theEvent);
	}
	// 
	else if (simData.GetInputMode() == SimData::InputMode::eUseEcoMug) {
		cout << "[WARNING] ConfigManager::ReadConfigurationFile: Selected InputMode = eUseEcoMug. An input number of particles is needed." << endl;
		ParticleFiller::FillParticleVector(cfg.fInputNParticles, theEvent);
	}
		
	cfg.fDetectorList  = tree.get<string>("DetectorList");
	cfg.fDetectorProperties = tree.get<string>("DetectorProperties");
	defProp.SetDefaultProperties(cfg.fDetectorProperties);

	cfg.fSimulationMode = tree.get<string>("Simulation.SimulationMode");
	simData.SetSimulationMode(simData.SimulationModeConversion(cfg.fSimulationMode));
	// cfg.fInjectionMode  = tree.get<string>("Simulation.InjectionMode");
	// simData.SetInjectionMode(simData.InjectionConversion(cfg.fInjectionMode));
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
	
	cout << "[INFO] ConfigManager::ReadDetectorList: Reading DetectorList File " << fDetectorList << endl;
	SimData& simData = theEvent.GetSimData();

	// used to determine maximum value of Z coordinate of current detectors in file
	double maxHeight = 0.;
	
	// read XML with detector positions
	ptree tree;
	read_xml(fDetectorList, tree);

	// injection settings
	string injectionBranch = "detectorList.injectionMode";
	// injection type
	string injModestr = tree.get<string>(injectionBranch + ".<xmlattr>.type");
	SimData::InjectionMode injMode = simData.InjectionConversion(injModestr);
	simData.SetInjectionMode(injMode);

	double posX = GetPropertyFromXML<double>(tree, injectionBranch, "x");
	double posY = GetPropertyFromXML<double>(tree, injectionBranch, "y");
	double posZ = GetPropertyFromXML<double>(tree, injectionBranch, "z");
	vector<double> injectionOrigin = {posX, posY, posZ}; 

 	// injection radius
	double defInjRadius = simData.GetInjectionRadius();
	double xmlInjRadius = GetPropertyFromXML<double>(tree, injectionBranch, "radius", defInjRadius);
	simData.SetInjectionRadius(xmlInjRadius);
	// injection height
	double defInjHeight = simData.GetInjectionHeight();
	double xmlInjHeight = GetPropertyFromXML<double>(tree, injectionBranch, "height", defInjHeight);
	simData.SetInjectionHeight(xmlInjHeight);
	// injection min theta
	double defMinTheta = simData.GetInjectionMinTheta();
	double xmlMinTheta = GetPropertyFromXML<double>(tree, injectionBranch, "minTheta", defMinTheta, false);
	simData.SetInjectionMinTheta(xmlMinTheta);
	// injection max theta
	double defMaxTheta = simData.GetInjectionMaxTheta();
	double xmlMaxTheta = GetPropertyFromXML<double>(tree, injectionBranch, "minTheta", defMaxTheta, false);
	simData.SetInjectionMaxTheta(xmlMaxTheta);
	// injection min phi
	double defMinPhi = simData.GetInjectionMinPhi();
	double xmlMinPhi = GetPropertyFromXML<double>(tree, injectionBranch, "minTheta", defMinPhi, false);
	simData.SetInjectionMinTheta(xmlMinPhi);
	// injection max phi
	double defMaxPhi = simData.GetInjectionMaxPhi();
	double xmlMaxPhi = GetPropertyFromXML<double>(tree, injectionBranch, "minTheta", defMaxPhi, false);
	simData.SetInjectionMaxPhi(xmlMaxPhi);
	

	for (const auto& i : tree.get_child("detectorList")) {
		ptree subtree;
		string name;
		tie(name, subtree) = i;

		// accessing detector settings
		if (name == "detector") {
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
			cout << "[INFO] Detector Position from XML = (" << detPosition.at(0) / CLHEP::cm << ", " << detPosition.at(1) / CLHEP::cm << ", " << detPosition.at(2) / CLHEP::cm << ") cm" << endl; 
			// search for another detector properties in the DetectorList.xml
			detector.SetDetectorProperties(subtree, defProp);
		}

	}

}


void
ConfigManager::PrintConfig(const Event::Config &cfg)
{
	cout << "[INFO] ConfigManager::PrintConfig:" << endl;
	cout << "Using the following configuration:" << endl;
	cout << "==================================" << endl;
	cout << " ------------- Input --------------" << endl;
	cout << "Input Mode = " << cfg.fInputMode << endl;

	if (cfg.fInputMode == "UseARTI") {
		cout << "InputFile = " << cfg.fInputFileName << endl;
	}
	else if (cfg.fInputMode == "UseEcoMug") {
		cout << "InputNParticles = " << cfg.fInputNParticles << endl;
	} else {
		cout << "Unknown input mode!" << endl;
	}

	cout << " ------------- Output -------------" << endl;
	cout << "OutputFile = " << cfg.fOutputFileName << endl;
	cout << "Save PE Time distribution (components) = " << (cfg.fSavePETimeDistribution ? "yes" : "no") << " (" << (cfg.fSaveComponentsPETimeDistribution ? "yes)" : "no)") << endl;
	cout << "Compress Output = " << (cfg.fCompressOutput ? "yes" : "no") << endl;
	cout << "Save Traces = " << (cfg.fSaveTraces ? "yes" : "no") << endl;
	cout << "Save Energy Deposit (components) = " << (cfg.fSaveEnergy ? "yes" : "no") << " (" << (cfg.fSaveComponentsEnergy ? "yes)" : "no)") << endl;
	cout << " ------------ Detector ------------" << endl;
	cout << "DetectorList = " << cfg.fDetectorList << endl;
	cout << "DetectorProperties = " << cfg.fDetectorProperties << endl;
	cout << " ---------- Simulation ------------" << endl;
	cout << "SimulationMode = " << cfg.fSimulationMode << endl;
	cout << "InjectionMode = " << cfg.fInjectionMode << endl;
	cout << "VisualizeGeometry = " << (cfg.fGeoVis ? "yes" : "no") << endl;
	cout << "VisualizeTrajectory = " << (cfg.fTrajVis ? "yes" : "no") << endl;
	cout << "RenderFile = " << cfg.fRenderFile << endl;
	cout << "PhysicsList = " << cfg.fPhysicsListName << endl;
	cout << "CheckOverlaps = " << (cfg.fCheckOverlaps ? "yes" : "no") << endl;
	cout << "==================================" << endl;
	cout << "==================================" << endl;
}