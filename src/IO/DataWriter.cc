#include "DataWriter.h"
#include "Particle.h"

#include "Detector.h"
#include "OptDevice.h"

#include "DetectorSimData.h"
#include "OptDeviceSimData.h"

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>


#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

using json = nlohmann::json;
using boost::iostreams::filtering_stream;
using boost::iostreams::filtering_streambuf;
using boost::iostreams::file_descriptor;
using boost::iostreams::output;
using namespace std;

Particle aParticle;

void 
DataWriter::FileWriter(Event& theEvent)
{	


	// load configuration	
	Event::Config &cfg = theEvent.GetConfig();
	
	string& outFileName = cfg.fOutputFileName;
	const bool& compressOutput = cfg.fCompressOutput;

	cout << "[INFO] DataWriter::FileWriter: Writing output to file " << outFileName << endl;

	// prepare json data holder
	json jData;

	SimData& simData = theEvent.GetSimData();
	
	if (cfg.fSaveInput) {
		cout << "[DEBUG] DataWriter::FileWriter: Saving input flux information!" << endl;
		SaveInputFlux(jData, simData);	
	}
	

	// loop over detector range
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {

		// get current detector data
		Detector& currDet = detIt->second;
		const unsigned int detId = currDet.GetId();
		
		if (!simData.HasDetectorSimData(detId))
			continue;

		cout << "[INFO] DataWriter::FileWriter: Accessing data of Detector ID: " << detId << endl;

		DetectorSimData& detSimData = simData.GetDetectorSimData(detId);

		/* 
			Detector level: access energy deposit, particle counters, etc... 
		*/
		if (cfg.fSaveEnergy)
			SaveEnergy(jData, simData, detId, cfg.fSaveComponentsEnergy);
		
		const int nOptDev = currDet.GetNOptDevice();
		cout << "[INFO] DataWriter::FileWriter: Detector ID: " << detId << " has " << nOptDev << " optical device(s)" << endl; 

		/* 
				Optical Device Level: Time traces, PE time distribution, charge, etc...
		*/
		// loop over optical devices 
		for (auto odIt = currDet.OptDeviceRange().begin(); odIt != currDet.OptDeviceRange().end(); odIt++) {

			OptDevice& currOd = odIt->second;
			const unsigned int odId = currOd.GetId();

			if (!detSimData.HasOptDeviceSimData(odId))
				continue;

			OptDeviceSimData& odSimData = detSimData.GetOptDeviceSimData(odId);

			// if has PE time distribution, has signals.
			if (!odSimData.HasPETimeDistribution()) {
				cout << "[INFO] DataWriter::FileWriter: photo-electron time distributions not found for Optical Device " << odId << endl;
				continue;
			}

			// save photo-electron time distribution(s)
			if (cfg.fSavePETimeDistribution)
				SavePETimeDistribution(jData, detSimData, odId, cfg.fSaveComponentsPETimeDistribution);

			// save time traces
			if (cfg.fSaveTraces)
				SaveTraces(jData, detSimData, odId);

		} // end loop over optical devices

	} // end loop over detectors

	// use compress if set in configuration
	if (compressOutput) {
		
		outFileName+=".gz";
		boost::iostreams::file_descriptor_sink outfile(outFileName);
		// apply compression filters
		boost::iostreams::filtering_streambuf<output> out;
		out.push(boost::iostreams::gzip_compressor());
    out.push(outfile);
    ostream outFd(&out);
    outFd << jData;
    outFd.flush();
    boost::iostreams::close(out);
    outfile.close();
	} 

	else {
		ofstream outfile(outFileName);
		outfile << jData;
	}
	
}

void
DataWriter::SaveInputFlux(json &jData, SimData& simData)
{
	vector<json> jParticles;

	for (auto pIt = simData.GetInjectedParticleVector().begin(); pIt != simData.GetInjectedParticleVector().end(); ++pIt) {
		json myCurrentParticle;
		Particle &part = *pIt;

		myCurrentParticle["ID"] = part.GetParticleId();
		myCurrentParticle["Position"] = part.GetPosition();
		myCurrentParticle["Momentum"] = part.GetMomentumDirection();

		jParticles.push_back(myCurrentParticle);

	}

	jData["InputFlux"] = jParticles;
}


void
DataWriter::SavePETimeDistribution(json &jData, const DetectorSimData& detSimData, int odId, const bool saveComponents)
{

	const OptDeviceSimData &odSimData = detSimData.GetOptDeviceSimData(odId);

	json jPETimeDistribution;
	json jComponentPETimeDistribution;


	if (saveComponents) {

		for (int compIt = Particle::eElectromagnetic; compIt < Particle::eEnd; compIt++) {

			Particle::Component particleComponent = static_cast<Particle::Component>(compIt);
			string componentName = aParticle.GetComponentName(particleComponent);

			if (!odSimData.HasPETimeDistribution(particleComponent)) {
				cout << "[WARNING] DataWriter::FileWriter: OptDevice " << odId << " has no PE time distribution for component " << componentName << endl;
				continue;
			}

			auto peTimedistributionRangeComp = odSimData.GetPETimeDistribution(particleComponent);

			jComponentPETimeDistribution[componentName] = peTimedistributionRangeComp;

		} // end loop particle components

		jData["OptDevice_"+to_string(odId)]["PETimeDistribution"] = jComponentPETimeDistribution;

	}
	// save PETimeDistributions when components are disabled. do not save both.
	else {

		auto peTimeDistribution = odSimData.GetPETimeDistribution();	
		jPETimeDistribution = peTimeDistribution;
		jData["OptDevice_"+to_string(odId)]["PETimeDistribution"] = jPETimeDistribution;
	}
	
}

void
DataWriter::SaveTraces(json &jData, const DetectorSimData& detSimData, int odId)
{
	json jTimeTraces;
	const OptDeviceSimData &odSimData = detSimData.GetOptDeviceSimData(odId);
	auto timeTraces = odSimData.GetTimeTraceDistribution();
	jTimeTraces = timeTraces;

	cout << "[INFO] DataWriter::SaveTraces: Saving time traces of Optical Device with ID " << odId << endl;
	jData["OptDevice_"+to_string(odId)]["TimeTraces"] = jTimeTraces;

}

void 
DataWriter::SaveEnergy(json &jData, const SimData& simData, int detId, const bool saveComponents)
{

	json jEnergyDeposit;
	json jEnergyDepositComponent;

	const DetectorSimData &detSimData = simData.GetDetectorSimData(detId);
	const vector<double> & energyDeposit = detSimData.GetEnergyDeposit();
	jEnergyDeposit["EnergyDeposit"] = energyDeposit;
	
	if (saveComponents) {
		
		

		for (int compIt = Particle::eElectromagnetic; compIt < Particle::eEnd; compIt++) {

		Particle::Component particleComponent = static_cast<Particle::Component>(compIt);
		string componentName = aParticle.GetComponentName(particleComponent);

		if (!detSimData.HasEnergyDeposit(particleComponent)) {
			cout << "[WARNING] DataWriter::SaveEnergy: Detector " << detId << " has no energy deposits of component " << componentName << endl;
			continue;
		}

		vector<double> energyDepositComponent = detSimData.GetEnergyDeposit(particleComponent);
		jEnergyDepositComponent["EnergyDeposit_"+componentName] = energyDepositComponent;

		} // end loop particle components

		jData["Detector_"+to_string(detId)]["DetectorSimData"] = jEnergyDepositComponent;	

	}
	

	jData["Detector_"+to_string(detId)]["DetectorSimData"] = (saveComponents ? jEnergyDepositComponent : jEnergyDeposit);
	

}