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
	cfg.fInputMode = tree.get<string>("Input.Mode", "UseARTI");
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
		
	cfg.fDetectorList  = tree.get<string>("DetectorList", "./DetectorList.xml");
	cfg.fDetectorProperties = tree.get<string>("DetectorProperties", "./DetectorProperties.xml");
	defProp.SetDefaultProperties(cfg.fDetectorProperties);

	cfg.fSimulationMode = tree.get<string>("Simulation.SimulationMode", "eFull");
	simData.SetSimulationMode(simData.SimulationModeConversion(cfg.fSimulationMode));
	
	cfg.fGeoVis  = tree.get<bool>("Simulation.GeoVisOn", false);
	cfg.fTrajVis = tree.get<bool>("Simulation.TrajVisOn", false);
	cfg.fCheckOverlaps = tree.get<bool>("Simulation.CheckOverlaps", false);
	cfg.fRenderFile = tree.get<string>("Simulation.RenderFile", "VRML2FILE");
	cfg.fPhysicsListName = tree.get<string>("Simulation.PhysicsName", "QGSP_BERT_HP");
	cfg.fVerbosity = tree.get<int>("Simulation.Verbosity", 1);

	cfg.fOutputFileName = tree.get<string>("Output.OutputFile");
	simData.SetOutputFileName(cfg.fOutputFileName);
	cfg.fCompressOutput = tree.get<bool>("Output.CompressOutput", true);
	cfg.fSaveInput = tree.get<bool>("Output.SaveInput", false);
	cfg.fSavePETimeDistribution = tree.get<bool>("Output.SavePETimeDistribution", false);
	cfg.fSaveComponentsPETimeDistribution = tree.get<bool>("Output.SaveComponentsPETimeDistribution", false);
	cfg.fSaveTraces     = tree.get<bool>("Output.SaveTraces", false);
	cfg.fSaveEnergy     = tree.get<bool>("Output.SaveEnergy", false);
	cfg.fSaveComponentsEnergy = tree.get<bool>("Output.SaveComponentsEnergy", false);
	cfg.fSaveCharge = tree.get<bool>("Output.SaveCharge", false);
	cfg.fSaveCounts = tree.get<bool>("Output.SaveCounts", false);

	return theEvent;

}

void
ConfigManager::ReadDetectorList(const string &fDetectorList, Event& theEvent)
{
	
	cout << "[INFO] ConfigManager::ReadDetectorList: Reading DetectorList File " << fDetectorList << endl;
	// print detector configuration
	cout << "Using the following detector configuration: " << endl;
	cout << "===========================================" << endl;

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
	cout << "---------- Particle Injection ------------- " << endl;
	cout << "Injection Mode: " << simData.GetInjectionModeName() << endl;

	double posX = GetPropertyFromXML<double>(tree, injectionBranch, "x");
	double posY = GetPropertyFromXML<double>(tree, injectionBranch, "y");
	double posZ = GetPropertyFromXML<double>(tree, injectionBranch, "z");
	vector<double> injectionOrigin = {posX, posY, posZ}; 
	simData.SetInjectionOrigin(injectionOrigin);
	cout << "Origin of injection coordinates: " << endl;
	cout << "(x0, y0, z0) = (" << posX / CLHEP::m << ", " << posY / CLHEP::m << ", " << posZ / CLHEP::m << ") m" << endl;

 	// injection radius
	double defInjRadius = simData.GetInjectionRadius();
	double xmlInjRadius = GetPropertyFromXML<double>(tree, injectionBranch, "radius", defInjRadius);
	simData.SetInjectionRadius(xmlInjRadius);
	cout << "Injection Radius = " << xmlInjRadius / CLHEP::m << " m" << endl;
	// injection height
	double defInjHeight = simData.GetInjectionHeight();
	double xmlInjHeight = GetPropertyFromXML<double>(tree, injectionBranch, "height", defInjHeight);
	simData.SetInjectionHeight(xmlInjHeight);
	cout << "Injection Height = " << xmlInjHeight / CLHEP::m << " m" << endl;
	// injection min theta
	double defMinTheta = simData.GetInjectionMinTheta();
	double xmlMinTheta = GetPropertyFromXML<double>(tree, injectionBranch, "minTheta", defMinTheta, false);
	simData.SetInjectionMinTheta(xmlMinTheta);
	cout << "Injection MinTheta = " << xmlMinTheta << " deg" << endl; 
	// injection max theta
	double defMaxTheta = simData.GetInjectionMaxTheta();
	double xmlMaxTheta = GetPropertyFromXML<double>(tree, injectionBranch, "maxTheta", defMaxTheta, false);
	simData.SetInjectionMaxTheta(xmlMaxTheta);
	cout << "Injection MaxTheta = " << xmlMaxTheta << " deg" << endl;
	// injection min phi
	double defMinPhi = simData.GetInjectionMinPhi();
	double xmlMinPhi = GetPropertyFromXML<double>(tree, injectionBranch, "minPhi", defMinPhi, false);
	simData.SetInjectionMinPhi(xmlMinPhi);
	cout << "Injection MinPhi = " << xmlMinPhi << " deg" << endl;
	// injection max phi
	double defMaxPhi = simData.GetInjectionMaxPhi();
	double xmlMaxPhi = GetPropertyFromXML<double>(tree, injectionBranch, "maxPhi", defMaxPhi, false);
	simData.SetInjectionMaxPhi(xmlMaxPhi);
	cout << "Injection MaxPhi = " << xmlMaxPhi << " deg" << endl;
	cout << "===========================================" << endl;
	cout << "===========================================" << endl;

	cout << "------------ Detector Info -------------- " << endl;
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

			cout << "Reading configuration of detector " << detTypestr << " with ID = " << detId << endl;
			// register detector in the Event
			if (!theEvent.HasDetector(detId)) {
				theEvent.MakeDetector(detId, detType);
			}
			else {
				cout << "[WARNING] Detector " << " with ID = " << detId << " already registered! IDs must be unique, please check your DetectorList.xml" << endl;
				continue;
			}
			
			Detector& detector = theEvent.GetDetector(detId);
			// set name from XML once the detector was created
			detector.SetName(detTypestr);
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
			cout << "Detector position " << endl;
			cout << "(x0, y0, z0) = (" << detPosition.at(0) / CLHEP::m << ", " << detPosition.at(1) / CLHEP::m << ", " << detPosition.at(2) / CLHEP::m << ") m" << endl; 
			// search for another detector properties in the DetectorList.xml
			detector.SetDetectorProperties(subtree, defProp);
		}

	}

	cout << "===========================================" << endl;
	cout << "===========================================" << endl;


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
	cout << "Save Input = " << (cfg.fSaveInput ? "yes" : "no") << endl;
	cout << "Save PE Time distribution (components) = " << (cfg.fSavePETimeDistribution ? "yes" : "no") << " (" << (cfg.fSaveComponentsPETimeDistribution ? "yes)" : "no)") << endl;
	cout << "Compress Output = " << (cfg.fCompressOutput ? "yes" : "no") << endl;
	cout << "Save Traces = " << (cfg.fSaveTraces ? "yes" : "no") << endl;
	cout << "Save Energy Deposit (components) = " << (cfg.fSaveEnergy ? "yes" : "no") << " (" << (cfg.fSaveComponentsEnergy ? "yes)" : "no)") << endl;
	cout << "Save Charge (number of PE) = " << (cfg.fSaveCharge ? "yes" : "no") << endl;
	cout << "Save Bar Counts = " << (cfg.fSaveCounts ? "yes" : "no") << endl;

	cout << " -------- Geant4 Settings ---------" << endl;
	cout << "VisualizeGeometry = " << (cfg.fGeoVis ? "yes" : "no") << endl;
	cout << "VisualizeTrajectory = " << (cfg.fTrajVis ? "yes" : "no") << endl;
	cout << "RenderFile = " << cfg.fRenderFile << endl;
	cout << "PhysicsList = " << cfg.fPhysicsListName << endl;
	cout << "CheckOverlaps = " << (cfg.fCheckOverlaps ? "yes" : "no") << endl;
	cout << "==================================" << endl;
	cout << "==================================" << endl;
}