#include "Event.h"
#include "Utilities.h"
#include "ParticleInjection.h"
#include "ConfigManager.h"
#include "G4MDetectorConstruction.h"
#include "G4MPhysicsList.h"
#include "G4MPrimaryGeneratorAction.h"
#include "ConfigFilePath.h"
#include "Scintillator.h"

// Geant4 headers
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4RunManagerFactory.hh"
#include "Randomize.hh"

// c++ headers
#include <iostream>
using boost::property_tree::ptree;


const string cConfigFilename = Utilities::ConcatenatePaths(GetConfigFilePath(), "Ut_G4AppSimulator.json");
const string cDetectorFilename = Utilities::ConcatenatePaths(GetConfigFilePath(), "Ut_DetectorList.xml");
ptree fTree;
const G4long cSeed = 123456789;

namespace
{   
	double threshold = 1e-10;
	double sanitizeZero(double value) {
		return (std::abs(value) < threshold) ? 0.0 : value;
	}

	void compareValues(double aExpected, double aActual, const string& testName, double tolerance = 1e-12) {
		if (std::abs(aExpected - aActual) >= tolerance) {
			std::stringstream errorMsg;
			errorMsg << "Values do not match for test " << testName << ". "
				 << "Expected: " << aExpected << ", Actual: " << aActual
				 << ", Tolerance: " << tolerance;	
			throw std::runtime_error(errorMsg.str());
		}
	}
	
	void compareInteger(int aExpected, int aActual, const string& testName) {
		if (aExpected != aActual) {	
			std::stringstream errorMsg;
			errorMsg << "Integer comparison failed for " << testName << ". "
				 << "Expected: " << aExpected
				 << ", Actual: " << aActual;
			throw std::runtime_error(errorMsg.str());
		}
	}

	void compareBool(bool aExpected, bool aActual, const string& testName) {
		if (aExpected != aActual) {	
			std::stringstream errorMsg;
			errorMsg << "Boolean comparison failed for " << testName << ". "
				 << "Expected: " << std::boolalpha << aExpected
				 << ", Actual: " << aActual;
			throw std::runtime_error(errorMsg.str());
		}
	}

	void compareString(string aExpected, string aActual, const string& testName) {
		if (aExpected != aActual) {	
			std::stringstream errorMsg;
			errorMsg << "String comparison failed for " << testName << ". "
				 << "Expected: " << aExpected
				 << ", Actual: " << aActual;
			throw std::runtime_error(errorMsg.str());
		}
	}

	bool RunSimulation(Event &aEvent) {
		// prepare simulation
		SimData& simData = aEvent.GetSimData();
		simData.SetSeed(cSeed);
		G4Random::setTheEngine(new CLHEP::RanecuEngine);
		G4Random::setTheSeed(cSeed);
		G4RunManager *runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
		runManager->SetVerboseLevel(0);

		// debug only
		G4UImanager *uiManager = 0;
		G4VisManager *visManager = 0;
		if(aEvent.GetConfig().fGeoVis) {
			uiManager = G4UImanager::GetUIpointer();
			visManager = new G4VisExecutive;
			
			visManager->Initialize();

			uiManager->ApplyCommand(("/vis/open " + aEvent.GetConfig().fRenderFile).c_str());
			uiManager->ApplyCommand("/vis/scene/create");
			uiManager->ApplyCommand("/vis/sceneHandler/attach");
			uiManager->ApplyCommand("/vis/scene/add/volume");
			uiManager->ApplyCommand("/vis/scene/add/axes 0 0 0 1 m");
			uiManager->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 0. 0.");
			uiManager->ApplyCommand("/vis/viewer/set/targetPoint 0 0 0");
			uiManager->ApplyCommand("/vis/viewer/zoom 1");
			uiManager->ApplyCommand("/vis/viewero/set/style/wireframe");
			uiManager->ApplyCommand("/vis/drawVolume");
			uiManager->ApplyCommand("/vis/scene/notifyHandlers");
			uiManager->ApplyCommand("/vis/viewer/update");
		}
		// 
		if(!runManager) {
			throw std::runtime_error("An error occurred initializing G4RunManager");
		}

		G4MDetectorConstruction *detConstruction = new G4MDetectorConstruction(aEvent);
		if(!detConstruction) {
			throw std::runtime_error("An error occurred initializing G4MDetectorConstruction");
		}
		auto * physWorld = detConstruction->Construct();
		Detector &currentDetector = aEvent.GetDetector(0);
		currentDetector.BuildDetector(physWorld->GetLogicalVolume(), aEvent);
		runManager->SetUserInitialization(detConstruction);
		runManager->SetUserInitialization(new G4MPhysicsList(simData.GetPhysicsListName()));

		G4MPrimaryGeneratorAction *prmGenAction = new G4MPrimaryGeneratorAction(aEvent);
		if(!prmGenAction) {
			throw std::runtime_error("An error occurred initializing G4MPrimaryGeneratorAction");
		}
		runManager->SetUserAction(prmGenAction);

		// initialize G4 kernel
		runManager->Initialize();
		for(auto it = simData.GetParticleVector().begin(); it != simData.GetParticleVector().end(); ++it) {
			simData.SetCurrentParticle(*it);
			runManager->BeamOn(1);
		}
		
		delete runManager;
		return true;
	}

	bool test_ReadConfigurationFile(Event& aEvent) {
		ConfigManager::ReadConfigurationFile(aEvent, cConfigFilename);
		Event::Config &cfg = aEvent.GetConfig();
		try {
			// input
			compareString("UseEcoMug", cfg.fInputMode, "InputMode");
			compareInteger(1, cfg.fInputNParticles, "InputNParticles");
			// output
			compareBool(true, cfg.fCompressOutput, "CompressOutput");
			compareString("test.json", cfg.fOutputFileName, "OutputFile");
			compareBool(false, cfg.fSavePETimeDistribution, "SavePETimeDistribution");
			compareBool(false, cfg.fSaveComponentsPETimeDistribution, "SaveComponentsPETimeDistribution");
			compareBool(true, cfg.fSaveEnergy, "SaveEnergy");
			compareBool(false, cfg.fSaveComponentsEnergy, "SaveComponentsEnergy");
			// simulation
			compareString("eFast", cfg.fSimulationMode, "SimulationMode");
			compareBool(false, cfg.fGeoVis, "GeoVis");
			compareBool(false, cfg.fTrajVis, "TrajVis");
			compareBool(false, cfg.fCheckOverlaps, "CheckOverlaps");
		} catch (std::exception &e) {
			Logger::Print("FAIL: " + std::string(e.what()), ERROR, "test_ReadConfigurationFile");
			return false;
		}
		return true;
	}

	bool test_ReadDetectorList(Event &aEvent) {

		try {
			SimData &simData = aEvent.GetSimData();
			// ConfigManager::SetupParticleInection(fTree, simData);
			ParticleInjection &injection = simData.GetParticleInjection();
			compareBool(true, injection.IsValid(), "InjectionValid");
			compareInteger(ParticleInjection::eVertical, injection.GetInjectionType(), "InjectionType");
			const std::vector<double> &injectionOrigin = injection.GetInjectionOrigin();
			compareValues(0.0 * CLHEP::m, injectionOrigin.at(0), "InjectionOrigin_0");
			compareValues(0.0 * CLHEP::m, injectionOrigin.at(1), "InjectionOrigin_1");
			compareValues(0.1 * CLHEP::m, injectionOrigin.at(2), "InjectionOrigin_2");
			// ConfigManager::SetupDetectorList(fTree, aEvent);
			// get instances of Detector with ID 0
			Detector& detector = aEvent.GetDetector(0);
			compareBool(true, detector.IsValid(), "DetectorValid");
			compareInteger(Detector::eScintillator, detector.GetType(), "DetectorType");
			std::vector<double> detPosition = detector.GetDetectorPosition();
			compareValues(-0.01 * CLHEP::m, detPosition.at(0), "DetectorPosition_0");
			compareValues( 0.02 * CLHEP::m, detPosition.at(1), "DetectorPosition_1");
			compareValues( 0.0  * CLHEP::m, detPosition.at(2), "DetectorPosition_2");
			Scintillator *scinDet = dynamic_cast<Scintillator*>(&detector);
			int nBars = scinDet->GetNBars();
			compareInteger(8, nBars, "ScintillatorBars");
			assert(nBars == 8);
			
		} catch (std::exception &e) {
			Logger::Print("FAIL: " + std::string(e.what()), ERROR, "test_ReadDetectorList");
			return false;
		}

		return true;
	}

	bool test_PrimaryGenerator(SimData& aSimData) {
		try {
			Particle currentParticle = aSimData.GetCurrentParticle();
			compareInteger(13, std::abs(currentParticle.GetParticleId()), "ParticleId");
			const std::vector<double> &injectionPosition = currentParticle.GetInjectionPosition();
			compareValues(0.0, sanitizeZero(injectionPosition.at(0)) / CLHEP::m, "InjectionPosition_0");
			compareValues(0.0, sanitizeZero(injectionPosition.at(1)) / CLHEP::m, "InjectionPosition_1");
			compareValues(0.1, sanitizeZero(injectionPosition.at(2)) / CLHEP::m, "InjectionPosition_2");
			const std::vector<double> &momentumDirection = currentParticle.GetMomentumDirection();
			compareValues(0.0, momentumDirection.at(0), "MomentumDirection_0");
			compareValues(0.0, momentumDirection.at(1), "MomentumDirection_1");
			compareValues(-449.164735774273652/*-837.874689031361299*/, momentumDirection.at(2), "MomentumDirection_2");
			compareValues(2.411469494153661/*2.257155077674716*/, currentParticle.GetZenith(), "Zenith");
			compareValues(3.592646135943527/*1.022436562227548*/, currentParticle.GetAzimuth(), "Azimuth");
			
		} catch (std::exception &e) {
			Logger::Print("FAIL: " + std::string(e.what()), ERROR, "test_PrimaryGenerator");
			return false;
		}
		return true;
	}

	bool test_G4Simulation(Event &aEvent) {
		try {
			bool success = RunSimulation(aEvent);
			compareBool(true, success, "RunSimulation");
			DetectorSimData &detSimData = aEvent.GetSimData().GetDetectorSimData(0);
			double depositedEnergy = detSimData.GetEnergyDeposit().at(0);
			compareValues(1.96, depositedEnergy, "DepositedEnergy", 1e-2);
		} catch (std::exception &e) {
			Logger::Print("FAIL: " + std::string(e.what()), ERROR, "test_G4Simulation");
			return false;
		}
		return true;
	}
}

int main(int argc, char** argv)
{
	bool configTestPass;
	bool injectionTestPass;
	bool simulationTestPass;
	bool primaryTestPass;

	read_json(cConfigFilename, fTree);
	Event event;
	configTestPass = test_ReadConfigurationFile(event);
	injectionTestPass = test_ReadDetectorList(event);
	simulationTestPass = test_G4Simulation(event);
	SimData &simData = event.GetSimData();
	primaryTestPass = test_PrimaryGenerator(simData);

	ostringstream msg;
	msg << "Summary of Unit Tests: " << endl;
	msg << "test_ReadConfigurationFile: " << (configTestPass ? "PASS" : "FAIL") << endl;
	msg << "test_ReadDetectorList: " << (injectionTestPass ? "PASS" : "FAIL") << endl;
	msg << "test_G4simulation: " << (simulationTestPass ? "PASS" : "FAIL") << endl;
	msg << "test_PrimaryGenerator: " << (primaryTestPass ? "PASS" : "FAIL") << endl;
	
	Logger::Print(msg, INFO);
	return 0;
}