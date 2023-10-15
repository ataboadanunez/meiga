#!/usr/bin/python3
import numpy as np
import matplotlib.pyplot as plt
# import SimDataReader to access data from output file
from SimDataReader import *

if __name__ == "__main__":

	import argparse
	parser = argparse.ArgumentParser(description="[INFO] This script reads output files from simulations with Meiga.", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("-f",
											dest="ofile",
											help="full path to the output file. Example: /home/user/meiga/G4WCDSimulator/output.json",
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

	# Get charge
	charge = odSimData.get_charge()

	print("[INFO] Merging input flux and detector data")
	# Merging input and output data
	merged_df = inputFlux.copy()
	merged_df['energy'] = depositedEnergy
	merged_df['charge'] = charge
	print(merged_df.head())

	# plot histogram of deposited energy
	fig = plt.figure()
	plt.hist(merged_df[merged_df.energy>0].energy, bins=100, histtype='step', color='k')
	plt.xlabel('Deposited Energy / MeV')
	plt.ylabel('Counts')
	plt.yscale('log')

	# plot histogram of charge
	fig = plt.figure()
	plt.hist(merged_df[merged_df.charge>0].charge, bins=100, histtype='step', color='k')
	plt.xlabel('Charge')
	plt.ylabel('Counts')
	plt.yscale('log')
	plt.show()

