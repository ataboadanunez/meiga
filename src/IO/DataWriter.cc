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
			at this level one could access energy deposit, particle counters, etc... 
			this is set in the configuration
		*/
		if (cfg.fSaveEnergy) {
			SaveEnergy(jData, simData, detId);
		}
		


		const int nOptDev = currDet.GetNOptDevice();
		cout << "[INFO] DataWriter::FileWriter: Detector ID: " << detId << " has " << nOptDev << " optical device(s)" << endl; 

		// loop over optical devices 
		for (auto odIt = currDet.OptDeviceRange().begin(); odIt != currDet.OptDeviceRange().end(); odIt++) {

			OptDevice& currOd = odIt->second;
			const unsigned int odId = currOd.GetId();

			if (!detSimData.HasOptDeviceSimData(odId))
				continue;

			OptDeviceSimData& odSimData = detSimData.GetOptDeviceSimData(odId);

			// checking for signals at optical device (i.e., photo-electron time distribution)
			const auto *peTimeDistributionRange = odSimData.PETimeDistributionRange();
			if (!peTimeDistributionRange) {
				cout << "[INFO] DataWriter::FileWriter: photo-electron time distributions not found for Optical Device " << odId << endl;
				continue;
			}

			/* at this level one could access total charge (number of photo-electrons), 
			time distributions (pe arrival time), pulses (time vs Amplitude) 
			and also obtain these for different particle components.

			*/

			// lines below should be in separate function...
			// if (saveComponentPETimeDistribution...)
			json jComponentPETimeDistribution;
			for (int compIt = Particle::eElectromagnetic; compIt < Particle::eEnd; compIt++) {

				Particle::Component particleComponent = static_cast<Particle::Component>(compIt);
				string componentName = aParticle.GetComponentName(particleComponent);
				cout << "[DEBUG] DataWriter::FileWriter: Accessing data of component " << componentName << endl;

				if (!odSimData.HasPETimeDistribution(particleComponent)) {
					cout << "[INFO] DataWriter::FileWriter: OptDevice " << odId << " has no PE time distribution for component " << componentName << endl;
					continue;
				}

				auto peTimedistributionRangeComp = odSimData.PETimeDistributionRange(particleComponent);

				jComponentPETimeDistribution[componentName] = peTimedistributionRangeComp;

			} // end loop particle components

			jData["OptDevice_"+to_string(odId)]["PETimeDistribution"] = jComponentPETimeDistribution;

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
DataWriter::SaveEnergy(json &jData, const SimData& simData, int detId)
{

	json jEnergyDeposit;
	json jEnergyDepositComponent;

	const DetectorSimData &detSimData = simData.GetDetectorSimData(detId);
	const vector<double> & energyDeposit = detSimData.GetEnergyDeposit();
	jEnergyDeposit["EnergyDeposit"] = energyDeposit;

	for (int compIt = Particle::eElectromagnetic; compIt < Particle::eEnd; compIt++) {

		Particle::Component particleComponent = static_cast<Particle::Component>(compIt);
		string componentName = aParticle.GetComponentName(particleComponent);

		if (!detSimData.HasEnergyDeposit(particleComponent)) {
			cout << "[INFO] DataWriter::SaveEnergy: Detector " << detId << " has no energy deposits of component " << componentName << endl;
			continue;
		}

		vector<double> energyDepositComponent = detSimData.GetEnergyDeposit(particleComponent);
		jEnergyDepositComponent[componentName] = energyDepositComponent;

	} // end loop particle components

	jData["Detector_"+to_string(detId)]["DetectorSimData"] = jEnergyDeposit;
	jData["Detector_"+to_string(detId)]["DetectorSimData"]["EnergyDeposit_components"] = jEnergyDepositComponent;

}