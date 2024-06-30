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

using json = nlohmann::ordered_json;
//using ordered_json = nlohmann::ordered_json;
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
	Logger::Print("Writing output to file " + outFileName, INFO, "FileWriter");

	// prepare json data holder
	json jData;

	SimData& simData = theEvent.GetSimData();

	// testing event (particle) loop
	for (auto pIt = simData.GetParticleVector().begin(); pIt != simData.GetParticleVector().end(); ++pIt) {
		
		
		Particle &part = *pIt;
		size_t eventId = std::distance(simData.GetParticleVector().begin(), pIt);
		
		// json container to store event information	
		json jEvent;

		if (cfg.fSaveInput) {
			SaveInputFlux(jEvent, simData, eventId);
		}

		
		// loop over Detector to extract data at Detector level (energy deposits, bar counters, etc...)
		for (auto & detPair : theEvent.DetectorRange()) {

			// get current detector data
			Detector& currDet = *(detPair.second);
			const unsigned int detId = currDet.GetId();
			
			if (!simData.HasDetectorSimData(detId))
				continue;

			// get DetectorSimData
			DetectorSimData& detSimData = simData.GetDetectorSimData(detId);

			// get deposited energy
			if (cfg.fSaveEnergy && detSimData.HasEnergyDeposit()) {
				SaveDepositedEnergy(jEvent, detSimData, detId, eventId);
			}
			
			// get particle counter (for scintillator detectors)
			if (cfg.fSaveCounts && detSimData.HasBinaryCounter()) {
				SaveBinaryCounter(jEvent, detSimData, detId, eventId);
			}

			if ((cfg.fSavePETimeDistribution || cfg.fSaveComponentsPETimeDistribution) || cfg.fSaveCharge) {

				// loop over opt devices to extract data at OptDevice level (charge, PE time distributions)
				for (auto odIt = currDet.OptDeviceRange().begin(); odIt != currDet.OptDeviceRange().end(); odIt++) {
					
					OptDevice& currOd = odIt->second;
					const unsigned int odId = currOd.GetId();

					// check if has sim data
					if (!detSimData.HasOptDeviceSimData(odId))
						continue;

					OptDeviceSimData& odSimData = detSimData.GetOptDeviceSimData(odId);

					// save charge (number of PE)					
					if (cfg.fSaveCharge && odSimData.HasCharge()) {
						SaveCharge(jEvent, odSimData, detId, odId, eventId);
					}

					// save PE time distribution
					if ((cfg.fSavePETimeDistribution || cfg.fSaveComponentsPETimeDistribution) && odSimData.HasPETimeDistribution()) {

						SavePETimeDistribution(jEvent, odSimData, detId, odId, eventId, cfg.fSaveComponentsPETimeDistribution, part.GetComponent());
						
					}


				} // end loop over optical devices
					
			} // if save optical device data

		} // end loop over detectors

		// store event data tree in main tree
		jData["Output"]["Event_"+to_string(eventId)] = jEvent;

	} // end loop over events (particles)


	// save detector information
	json jDetectorList;
	for (auto & detPair : theEvent.DetectorRange()) {

		// get pointer to current detector
		Detector& currDet = *(detPair.second);
		const unsigned int detId = currDet.GetId();

		json jcurrentDetector;
		jcurrentDetector["ID"] = detId;
		jcurrentDetector["Name"] = currDet.GetName();
		jcurrentDetector["Position"] = currDet.GetDetectorPosition();
		
		// get list of optical device IDs
		vector<int> optDeviceList;
		for (auto odIt = currDet.OptDeviceRange().begin(); odIt != currDet.OptDeviceRange().end(); odIt++) {
			OptDevice& currOd = odIt->second;
			int odId = currOd.GetId();
			optDeviceList.push_back(odId);
		}

		jcurrentDetector["OptDeviceList"] = optDeviceList;
		
		jDetectorList["Detector_"+to_string(detId)] = jcurrentDetector;

	}

	jData["DetectorList"] = jDetectorList;


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


	return;
	
}

void
DataWriter::SaveInputFlux(json& jEvent, SimData& simData, size_t evid)
{
	
	json myCurrentParticle;
	Particle &part = simData.GetInjectedParticleVector().at(evid);

	myCurrentParticle["ID"] = part.GetParticleId();
	myCurrentParticle["Position"] = part.GetInjectionPosition();
	myCurrentParticle["Momentum"] = part.GetMomentumDirection();

	jEvent["InputFlux"] = myCurrentParticle;

}


void 
DataWriter::SaveDepositedEnergy(json &jEvent, const DetectorSimData& detSimData, int detId, size_t eventId)
{


	// get vector of energy deposits
	const auto & energyDeposit = detSimData.GetEnergyDeposit(); 
	jEvent["Detector_"+to_string(detId)]["EnergyDeposit"] = energyDeposit.at(eventId);

}


void
DataWriter::SavePETimeDistribution(json& jEvent, const OptDeviceSimData& odSimData, int detId, int odId, size_t eventId, const bool saveComponents, const Particle::Component comp)
{

	// get total component pe time distribution
	auto peTimeDistribution = odSimData.GetPETimeDistribution().at(eventId);

	// save muon-decayed signals
	if (saveComponents) {

		string componentName = aParticle.GetComponentName(comp);

		// skip if component has no PE time distribution
		if (!odSimData.HasPETimeDistribution(comp))
			return;

		jEvent["Detector_"+to_string(detId)]["OptDevice_"+to_string(odId)]["PETimeDistribution_"+componentName] = peTimeDistribution;

		// treat muons separate to save muon-decay signals
		if (comp == Particle::eMuonic) {

			// get PE time distribution of the decayed muon
			auto peTimeDistributionMuDec = odSimData.GetPETimeDistribution(Particle::eMuonDecay).at(eventId)
		;
			jEvent["Detector_"+to_string(detId)]["OptDevice_"+to_string(odId)]["PETimeDistribution_eMuonDecay"] = peTimeDistributionMuDec;
		
		}

	
	} else {
		jEvent["Detector_"+to_string(detId)]["OptDevice_"+to_string(odId)]["PETimeDistribution"] = peTimeDistribution;
	}

}

void
DataWriter::SaveCharge(json& jEvent, const OptDeviceSimData& odSimData, int detId, int odId, size_t eventId)
{

	jEvent["Detector_"+to_string(detId)]["OptDevice_"+to_string(odId)]["Charge"] = odSimData.GetCharge().at(eventId);

}

void
DataWriter::SaveBinaryCounter(json& jEvent, const DetectorSimData &detSimData, int detId, size_t eventId)
{
	
	const auto & binaryCounter = detSimData.GetBinaryCounter();
	jEvent["Detector_"+to_string(detId)]["BinaryCounter"] = binaryCounter.at(eventId);

}