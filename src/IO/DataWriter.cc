#include "DataWriter.h"
#include "Particle.h"

#include "Detector.h"
#include "OptDevice.h"

#include "SimData.h"
#include "DetectorSimData.h"
#include "OptDeviceSimData.h"

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

Particle aParticle;

void 
DataWriter::FileWriter(Event& theEvent)
{	

	SimData& simData = theEvent.GetSimData();
	const string& outFileName = simData.GetOutputFileName();

	cout << "[INFO] DataWriter::FileWriter: Writing output to file " << outFileName << endl;

	/*
		Configuration for output is needed in order to save
		desired data.
	*/	

	// prepare json data holder
	json jData;
	json jEnergyDeposit;
	// loop over detector range
	for (auto detIt = theEvent.DetectorRange().begin(); detIt != theEvent.DetectorRange().end(); detIt++) {

		// get current detector data
		Detector& currDet = detIt->second;
		const unsigned int detId = currDet.GetId();
		
		if (!simData.HasDetectorSimData(detId))
			continue;

		cout << "[INFO] DataWriter::FileWriter: Accessing data of Detector ID: " << detId << endl;

		DetectorSimData& detSimData = simData.GetDetectorSimData(detId);

		/* at this level one could access energy deposit, particle counters, etc... and this should be managed by a configuration
		*/
		std::vector<double> energyDeposit = detSimData.GetEnergyDeposit();
		jEnergyDeposit["EnergyDeposit"] = energyDeposit;

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

		jData["Detector_"+to_string(detId)]["DetectorSimData"] = jEnergyDeposit;

	} // end loop over detectors

	ofstream outfile(outFileName);
	outfile << jData;

}