// Meiga headers
#include "ConfigManager.h"
#include "ParticleFiller.h"
#include "Utilities.h"
#include "ParticleInjection.h"
#include "Logger.h"
// Geant4 headers
#include "G4UnitsTable.hh"

using boost::property_tree::ptree;
using namespace std;

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
	else {
		ostringstream msg;
		msg << "Unknown input mode " << cfg.fInputMode << ". Valid types are: eUserARTI, eUseEcoMug";
		throw invalid_argument(msg.str());
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

	SetupParticleInection(tree, simData);
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
	// used to determine maximum value of Z coordinate of current detectors in file
	double maxHeight = 0.;
	
	// read XML with detector positions
	ptree tree;
	read_xml(fDetectorList, tree);
	
	Logger::Print(msg, INFO, "ReadDetectorList");
	std::cout << "------------ Detector Info -------------- " << "\n";
	for (const auto& det : tree.get_child("detectorList")) {
		if (det.first == "detector") {
			string detIdstr = det.second.get<string>("<xmlattr>.id");
			string detTypestr = det.second.get<string>("<xmlattr>.type");
			int detId = stoi(detIdstr);
			Detector::DetectorType detType = Detector::StringToType(detTypestr);
			std::cout << "Reading configuration of detector " << detTypestr << " with ID = " << detId << "\n";
			// register detector in the Event
			if (!theEvent.HasDetector(detId)) {
				theEvent.MakeDetector(detId, detType);
			}
			else {
				Logger::Print("Detector with ID = "+detIdstr+" already registered! IDs must be unique, please check your DetectorList.xml", WARNING, "ReadDetectorList");
				continue;
			}
			
			Detector& detector = theEvent.GetDetector(detId);
			detector.SetDetectorProperties(det.second);
		}

	}
	std::cout << "===========================================" << endl;
	std::cout << "===========================================" << endl;
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

void ConfigManager::SetupParticleInection(const ptree &tree, SimData &aSimData)
{
	const auto& injectionTree = tree.get_child("ParticleInjection");
	// Retrieve the type
	std::string type;
	try {
		type = injectionTree.get<std::string>("Type");
	} catch (const boost::property_tree::ptree_bad_path& e) {
		Logger::Print("Type field not found in configuration file: " + std::string(e.what()), ERROR, "SetupParticleInjection");
	} catch (const std::exception &e) {
		Logger::Print("An error occurred while retrieveing injection type: " + std::string(e.what()), ERROR, "SetupParticleInjection");
	}
	
	// Create a ParticleInjection object with these values
	ParticleInjection injection(type);

	// Retrieve the origin as a vector
	auto originNode = injectionTree.get_child_optional("Origin");
	if (originNode) {
		std::vector<double> origin;
		for (const auto& item: originNode.get()) {
			origin.push_back(item.second.get_value<double>() * CLHEP::cm);
		}
		injection.SetInjectionOrigin(origin);
	}
	
	// additional injection parameters for eCircle and eHalfSphere injection types
	auto radius = injectionTree.get_optional<double>("Radius");
	if (radius.has_value()) {
		injection.SetInjectionRadius(radius.get() * CLHEP::cm);
	}
	auto minTheta = injectionTree.get_optional<double>("MinTheta");
	if (minTheta.has_value())
		injection.SetInjectionMinTheta(minTheta.get());
	auto maxTheta = injectionTree.get_optional<double>("MaxTheta");
	if (maxTheta.has_value())
		injection.SetInjectionMaxTheta(maxTheta.get());
	auto minPhi = injectionTree.get_optional<double>("MinPhi");
	if (minPhi.has_value())
		injection.SetInjectionMinPhi(minPhi.get());
	auto maxPhi = injectionTree.get_optional<double>("MaxPhi");
	if (maxPhi.has_value())
		injection.SetInjectionMaxPhi(maxPhi.get());

	// Now you can use the 'injection' object as needed
	if (injection.IsValid()) {
		aSimData.SetParticleInjection(injection);
	} else {
		throw invalid_argument("Invalid ParticleInjection parameters were found in the configuration file.");
	}
}
