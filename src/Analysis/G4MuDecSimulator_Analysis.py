#!/usr/bin/python3
import numpy as np
import matplotlib.pyplot as plt
# import SimDataReader to access data from output file
from SimDataReader import *
from IPython import embed



if __name__ == "__main__":

	import argparse
	parser = argparse.ArgumentParser(description="[INFO] This script reads output files from simulations with Meiga.", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("-f",
											dest="ofile",
											help="full path to the output file. Example: meiga/build/ApplicationsG4MuDecSimulator/output.json.gz",
											default=None, required=False)

	options = parser.parse_args()
	outfile = options.ofile

	print(parser.description)
	if outfile is None:
		print("[ERROR] Path to the output file is needed!")
		exit(1)
	print("[INFO] Reading file %s" %outfile)
	print("\n")

	# Accessing Simulated data
	simData = SimDataReader(outfile)
	
	# Get list of event ids
	event_ids = simData.get_event_list()
	print("[INFO] Total number of events in file = %i" %len(event_ids))
	print("[INFO] Accessing data of InputFlux")
	# Get input flux DataFrame
	inputFlux = simData.get_input_flux()
	print(inputFlux.head())
	
	print("\n")
	print("[INFO] Accessing data from DetectorList")
	# Get detector list
	detectorList = simData.get_detector_list()
	print(detectorList)
	
	# Detector IDs
	detector_ids = detectorList.loc['ID']
	print("List of Detector IDs: ", detector_ids)

	print("\n")
	print("[INFO] Accessing Simulated data of Detector with ID = 0")
	# Accessing Simulated data of Detector 0
	detectorSimData = simData.GetDetectorSimData(det_id=0)
	# Get vector of deposited energy
	depositedEnergy = detectorSimData.get_deposited_energy()
	print("[INFO] Deposited energy = ", depositedEnergy)
	print("\n")

	# Accessing data from optical device
	odSimData = detectorSimData.GetOptDeviceSimData(od_id=0)

	# Access component photo-electron time distributions (traces)
	traces = defaultdict(list)
	traces['eMuonic'] = odSimData.get_pe_time_distribution(component='eMuonic')
	traces['eElectromagnetic'] = odSimData.get_pe_time_distribution(component='eElectromagnetic')
	traces['eHadronic'] = odSimData.get_pe_time_distribution(component='eHadronic')
	traces['eMuonDecay'] = odSimData.get_pe_time_distribution(component='eMuonDecay')

	# Compute charge as lenght of each trace
	charge = defaultdict(list)
	# iterate over events
	for evid in event_ids:

		charge['eMuonic'].append(len(traces['eMuonic'][evid]))
		charge['eElectromagnetic'].append(len(traces['eElectromagnetic'][evid]))
		charge['eHadronic'].append(len(traces['eHadronic'][evid]))
		charge['eMuonDecay'].append(len(traces['eMuonDecay'][evid]))

	# convert dictionaries to Pandas DataFrame
	traces_df = pd.DataFrame(traces)
	charge_df = pd.DataFrame(charge)
	embed()
	# Merging input and output data
	print("[INFO] Merging input flux and detector signals")
	
	new_df = inputFlux.copy()
	new_df['DepositedEnergy'] = depositedEnergy
	
	merged_df = pd.concat([new_df, charge_df], axis=1, join='inner')


	# plot histogram of deposited energy
	fig = plt.figure()
	plt.hist(merged_df[merged_df.DepositedEnergy>0].DepositedEnergy, bins=100, histtype='step', color='k')
	plt.xlabel('Deposited Energy / MeV')
	plt.ylabel('Counts')
	plt.yscale('log')

	# plot charge histogram per components
	fig = plt.figure()
	plt.hist(merged_df[merged_df.eMuonic > 0].eMuonic, bins=100, lw=1.8, histtype='step', label='Muonic')
	plt.hist(merged_df[merged_df.eElectromagnetic > 0].eElectromagnetic, bins=100, lw=1.8, histtype='step', label='Electromagnetic')
	plt.hist(merged_df[merged_df.eHadronic > 0].eHadronic, bins=100, lw=1.8, histtype='step', label='Hadronic')
	plt.hist(merged_df[merged_df.eMuonDecay > 0].eMuonDecay, bins=100, lw=1.8, histtype='step', label='MuonDecay')
	plt.xlabel('Number of photo-electrons')
	plt.ylabel('Counts')
	plt.yscale('log')
	plt.legend()

	plt.show()

