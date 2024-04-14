#!/usr/bin/python3
import numpy as np
import matplotlib.pyplot as plt
# import SimDataReader to access data from output file
from SimDataReader import *


def ConvertBinaryPixel(value, matrix, nBars=12):
	"""
	Add description
	"""	
	binary_x = value[0:nBars]
	binary_y = value[nBars:2*nBars]

	# find pixel number iterating from last element
	for i, itemi in enumerate(binary_x):
		for j, itemj in enumerate(binary_y):
			if (itemi == '1') & (itemj == '1'):
				matrix[i][j] += 1

	return matrix


if __name__ == "__main__":

	import argparse
	parser = argparse.ArgumentParser(description="[INFO] This script reads output files from simulations with Meiga.", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("-f",
											dest="ofile",
											help="full path to the output file. Example: meiga/build/Applications/G4HodoscopeSimulator/output.json.gz",
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

	# Iterate over detectors, get deposited energy on each detector and merge with input flux into one dataframe

	# Append deposited energy to input flux DataFrame
	# keep detector id in the column name

	print("[INFO] Merging input flux and deposited energy data")

	merged_df = inputFlux.copy()

	for detId in detector_ids:
		detSimData = simData.GetDetectorSimData(det_id=detId)
		edep = detSimData.get_deposited_energy()
		barCounter = detSimData.get_binary_counter()		
		merged_df['Detector_%i/DepositedEnergy' %detId] = edep
		merged_df['Detector_%i/BinaryCounter' %detId] = barCounter
		
	print(merged_df.head())


	# Create plot of Bar counters
	
	# get binary counters for each detector from dataframe
	binaryCounter_det_0 = merged_df['Detector_0/BinaryCounter']
	binaryCounter_det_1 = merged_df['Detector_1/BinaryCounter']
	binaryCounter_det_2 = merged_df['Detector_2/BinaryCounter']

	# prepare matrix for each detector
	plane_0 = np.zeros((12,12))
	plane_1 = np.zeros((12,12))
	plane_2 = np.zeros((12,12))

	n_events = len(event_ids)

	# iterate over events and compute counts for each plane
	for i in range(n_events):

		binary_0 = binaryCounter_det_0[i]
		binary_1 = binaryCounter_det_1[i]
		binary_2 = binaryCounter_det_2[i]

		# Check if there is coincidence in 3 planes
		if (ConvertBinaryPixel(binary_0, np.zeros((12,12))).sum() and ConvertBinaryPixel(binary_1, np.zeros((12,12))).sum() and ConvertBinaryPixel(binary_2, np.zeros((12,12))).sum()):
			plane_0 = ConvertBinaryPixel(binary_0, plane_0)
			plane_1 = ConvertBinaryPixel(binary_1, plane_1)
			plane_2 = ConvertBinaryPixel(binary_2, plane_2)

	# create plots
	fig = plt.figure()
	im_0 = plt.imshow(plane_0, interpolation='None', cmap=plt.cm.viridis)
	plt.colorbar(im_0, orientation='vertical', label=r'Counts')
	plt.xlabel(r'Bar number')
	plt.ylabel(r'Bar number')

	plt.xticks(ticks=[1, 3, 5, 7, 9, 11], labels=[2, 4, 6, 8, 10, 12])
	plt.yticks(ticks=[1, 3, 5, 7, 9, 11], labels=[2, 4, 6, 8, 10, 12])

	plt.title(r'Counts of Detector 0')

	fig = plt.figure()
	im_1 = plt.imshow(plane_1, interpolation='None', cmap=plt.cm.viridis)
	plt.colorbar(im_1, orientation='vertical', label=r'Counts')
	plt.xlabel(r'Bar number')
	plt.ylabel(r'Bar number')

	plt.xticks(ticks=[1, 3, 5, 7, 9, 11], labels=[2, 4, 6, 8, 10, 12])
	plt.yticks(ticks=[1, 3, 5, 7, 9, 11], labels=[2, 4, 6, 8, 10, 12])

	plt.title(r'Counts of Detector 1')

	fig = plt.figure()
	im_2 = plt.imshow(plane_2, interpolation='None', cmap=plt.cm.viridis)
	plt.colorbar(im_1, orientation='vertical', label=r'Counts')
	plt.xlabel(r'Bar number')
	plt.ylabel(r'Bar number')

	plt.xticks(ticks=[1, 3, 5, 7, 9, 11], labels=[2, 4, 6, 8, 10, 12])
	plt.yticks(ticks=[1, 3, 5, 7, 9, 11], labels=[2, 4, 6, 8, 10, 12])

	plt.title(r'Counts of Detector 2')

	


	# plot histogram of deposited energy at each detector
	edep_0 = merged_df['Detector_0/DepositedEnergy']
	edep_1 = merged_df['Detector_1/DepositedEnergy']
	edep_2 = merged_df['Detector_2/DepositedEnergy']
	fig = plt.figure()
	plt.hist(edep_0[edep_0>0.1], bins=50, lw=1.8, histtype='step', label='Detector 0')
	plt.hist(edep_1[edep_1>0.1], bins=50, lw=1.8, histtype='step', label='Detector 1')
	plt.hist(edep_2[edep_2>0.1], bins=50, lw=1.8, histtype='step', label='Detector 2')

	plt.xlabel('Deposited Energy / MeV')
	plt.ylabel('Counts')
	plt.legend()

	plt.show()