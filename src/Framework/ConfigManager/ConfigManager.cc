// Meiga headers
#include "ConfigManager.h"
#include "ParticleFiller.h"
#include "Utilities.h"
#include "Logger.h"
// Geant4 headers
#include "G4UnitsTable.hh"

using boost::property_tree::ptree;
using namespace std;

/** Static class member definition **/
DefaultProperties ConfigManager::defProp;

void
ConfigManager::ReadConfigurationFile(Event &aEvent, const string &fConfigFile)
{
	Logger::Print("Reading configuration file: " + fConfigFile, INFO, "ReadConfigurationFile");
	
	// Event theEvent;
	Event::Config &cfg = aEvent.GetConfig();
	ptree tree;
	read_json(fConfigFile, tree);
	
	cfg.fConfigurationFileName = fConfigFile;
	SimData& simData = aEvent.GetSimData();
	// input
	cfg.fInputMode = tree.get<string>("Input.Mode", "UseARTI");
	simData.SetInputMode(simData.InputModeConversion(cfg.fInputMode));
	cfg.fInputFileName = tree.get<string>("Input.InputFileName", "");
	cfg.fInputNParticles = tree.get<unsigned int>("Input.InputNParticles", 0);
	
	// reads the input (ARTI) file and fills the Event with particles
	if (simData.GetInputMode() == SimData::InputMode::eUseARTI) {
		Logger::Print("Selected InputMode = eUseARTI. An input file is needed.", WARNING, "ReadConfigurationFile");
		ParticleFiller::FillParticleVector(cfg.fInputFileName, aEvent);
	}
	// 
	else if (simData.GetInputMode() == SimData::InputMode::eUseEcoMug) {
		Logger::Print("Selected InputMode = eUseEcoMug. An input number of particles is needed.", WARNING, "ReadConfigurationFile");
		ParticleFiller::FillParticleVector(cfg.fInputNParticles, aEvent);
	}
		
	cfg.fDetectorList  = tree.get<string>("DetectorList", "./DetectorList.xml");

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
}

void
ConfigManager::ReadDetectorList(const string &fDetectorList, Event& theEvent)
{
	ostringstream msg;
	msg << "Reading DetectorList File " << fDetectorList << "\n";
	// print detector configuration
	msg << "Using the following detector configuration: " << "\n";
	msg << "===========================================" << "\n";

	SimData& simData = theEvent.GetSimData();

	// set default detector properties before detector construction
	defProp.SetDefaultProperties();

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
	msg << "---------- Particle Injection ------------- " << "\n";
	msg << "Injection Mode: " << simData.GetInjectionModeName() << "\n";

	double posX = GetPropertyFromXML<double>(tree, injectionBranch, "x");
	double posY = GetPropertyFromXML<double>(tree, injectionBranch, "y");
	double posZ = GetPropertyFromXML<double>(tree, injectionBranch, "z");
	vector<double> injectionOrigin = {posX, posY, posZ}; 
	simData.SetInjectionOrigin(injectionOrigin);
	msg << "Origin of injection coordinates: " << "\n";
	msg << "(x0, y0, z0) = (" << posX / CLHEP::m << ", " << posY / CLHEP::m << ", " << posZ / CLHEP::m << ") m" << "\n";

 	// injection radius
	double defInjRadius = simData.GetInjectionRadius();
	double xmlInjRadius = GetPropertyFromXML<double>(tree, injectionBranch, "radius", defInjRadius);
	simData.SetInjectionRadius(xmlInjRadius);
	msg << "Injection Radius = " << xmlInjRadius / CLHEP::m << " m" << "\n";
	// injection height
	double defInjHeight = simData.GetInjectionHeight();
	double xmlInjHeight = GetPropertyFromXML<double>(tree, injectionBranch, "height", defInjHeight);
	simData.SetInjectionHeight(xmlInjHeight);
	msg << "Injection Height = " << xmlInjHeight / CLHEP::m << " m" << "\n";
	// injection min theta
	double defMinTheta = simData.GetInjectionMinTheta();
	double xmlMinTheta = GetPropertyFromXML<double>(tree, injectionBranch, "minTheta", defMinTheta, false);
	simData.SetInjectionMinTheta(xmlMinTheta);
	msg << "Injection MinTheta = " << xmlMinTheta << " deg" << "\n"; 
	// injection max theta
	double defMaxTheta = simData.GetInjectionMaxTheta();
	double xmlMaxTheta = GetPropertyFromXML<double>(tree, injectionBranch, "maxTheta", defMaxTheta, false);
	simData.SetInjectionMaxTheta(xmlMaxTheta);
	msg << "Injection MaxTheta = " << xmlMaxTheta << " deg" << "\n";
	// injection min phi
	double defMinPhi = simData.GetInjectionMinPhi();
	double xmlMinPhi = GetPropertyFromXML<double>(tree, injectionBranch, "minPhi", defMinPhi, false);
	simData.SetInjectionMinPhi(xmlMinPhi);
	msg << "Injection MinPhi = " << xmlMinPhi << " deg" << "\n";
	// injection max phi
	double defMaxPhi = simData.GetInjectionMaxPhi();
	double xmlMaxPhi = GetPropertyFromXML<double>(tree, injectionBranch, "maxPhi", defMaxPhi, false);
	simData.SetInjectionMaxPhi(xmlMaxPhi);
	msg << "Injection MaxPhi = " << xmlMaxPhi << " deg" << "\n";
	msg << "===========================================" << "\n";
	msg << "===========================================" << "\n";
	Logger::Print(msg, INFO, "ReadDetectorList");
	cout << "------------ Detector Info -------------- " << "\n";
	for (const auto& i : tree.get_child("detectorList")) {
		ostringstream msg2;
		ptree subtree;
		string name;
		tie(name, subtree) = i;

		// accessing detector settings
		if (name == "detector") {
			vector<double> detPosition;
			string detIdstr = subtree.get<string>("<xmlattr>.id");
			string detTypestr = subtree.get<string>("<xmlattr>.type");
			int detId = stoi(detIdstr);
			Detector::DetectorType detType = Detector::StringToType(detTypestr);
			detPosition.clear();
			msg2 << "Reading configuration of detector " << detTypestr << " with ID = " << detId << "\n";
			// register detector in the Event
			if (!theEvent.HasDetector(detId)) {
				theEvent.MakeDetector(detId, detType);
			}
			else {
				Logger::Print("Detector with ID = "+detIdstr+" already registered! IDs must be unique, please check your DetectorList.xml", WARNING, "ReadDetectorList");
				continue;
			}
			
			Detector& detector = theEvent.GetDetector(detId);
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
			msg2 << "Detector position: " << "(x0, y0, z0) = (" << detPosition.at(0) / CLHEP::m << ", " 
																<< detPosition.at(1) / CLHEP::m << ", " 
																<< detPosition.at(2) / CLHEP::m << ") m";
			Logger::Print(msg2, INFO);
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
	ostringstream msg;
	msg << "Using the following configuration:" << endl;
	msg << "==================================" << endl;
	msg << " ------------- Input --------------" << endl;
	msg << "Input Mode = " << cfg.fInputMode << endl;

	if (cfg.fInputMode == "UseARTI") {
		msg << "InputFile = " << cfg.fInputFileName << endl;
	}
	else if (cfg.fInputMode == "UseEcoMug") {
		msg << "InputNParticles = " << cfg.fInputNParticles << endl;
	} else {
		msg << "Unknown input mode!" << endl;
	}

	msg << " ------------- Output -------------" << endl;
	msg << "OutputFile = " << cfg.fOutputFileName << endl;
	msg << "Compress Output = " << (cfg.fCompressOutput ? "yes" : "no") << endl;
	msg << "Save Input = " << (cfg.fSaveInput ? "yes" : "no") << endl;
	msg << "Save PE Time distribution (components) = " << (cfg.fSavePETimeDistribution ? "yes" : "no") << " (" << (cfg.fSaveComponentsPETimeDistribution ? "yes)" : "no)") << endl;
	msg << "Save Energy Deposit (components) = " << (cfg.fSaveEnergy ? "yes" : "no") << " (" << (cfg.fSaveComponentsEnergy ? "yes)" : "no)") << endl;
	msg << "Save Charge = " << (cfg.fSaveCharge ? "yes" : "no") << endl;
	msg << "Save Bar Counts = " << (cfg.fSaveCounts ? "yes" : "no") << endl;

	msg << " -------- Geant4 Settings ---------" << endl;
	msg << "VisualizeGeometry = " << (cfg.fGeoVis ? "yes" : "no") << endl;
	msg << "VisualizeTrajectory = " << (cfg.fTrajVis ? "yes" : "no") << endl;
	msg << "RenderFile = " << cfg.fRenderFile << endl;
	msg << "PhysicsList = " << cfg.fPhysicsListName << endl;
	msg << "CheckOverlaps = " << (cfg.fCheckOverlaps ? "yes" : "no") << endl;
	msg << "==================================" << endl;
	msg << "==================================" << endl;
	Logger::Print(msg, INFO, "PrintConfig");
}