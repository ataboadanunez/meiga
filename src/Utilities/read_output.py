# Python code to read Meiga output
import json
import gzip
import argparse
import numpy as np
import pandas as pd
import pickle

from collections import defaultdict
import os
from glob import glob


import matplotlib.pyplot as plt

from IPython import embed



def main():

	"""
	Description of the main function.

	This script is not intended for analysis.
	It is intended to be used as a guide
	for reading output files of the simulation.

	Inputs:
		- simulation output file (output.json)
		- configuration file (Cfg.json)
	Returns:
		Statistics and plots of charge histograms / energy deposits

	"""


	parser = argparse.ArgumentParser(description="This script reads output files from simulations with Meiga.", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("-i", 
											dest="ifile", 
											help="full path of the Application's output file. Exaple: /home/user/meiga/Application/output.json.gz", 
											default=None, required=True)
	parser.add_argument("-c",
											dest="cfg",
											help="full path of the Application's configuration file. Example: /home/user/meiga/Application/G4Application.json",
											default=None, required=True)

	options = parser.parse_args()

	ifile = options.ifile
	cfgfile = options.cfg

	# check that ifile and cfg exists
	if not (os.path.exists(ifile)):
		print("[ERROR] Input file %s does not exist. Wrong path or misspelled filename." %ifile)
		return

	if not (os.path.exists(cfgfile)):
		print("[ERROR] Configuration file %s does not exist. Wrong path or misspelled filename." %cfgfile)
		return

	print("[INFO] Reading configuration file: %s" %cfgfile)
	cfgfile2 = open(cfgfile, "rt")
	cfgdata = json.load(cfgfile2)
	outputcfg = cfgdata['Output']
	print("[INFO] Following options found in configuration file: ")
	print(outputcfg)

	# reading application output options from configuration file
	compressOutput = outputcfg.get('CompressOutput')
	saveInput = outputcfg.get('SaveInput')
	savePETimeDistribution = outputcfg.get('SavePETimeDistribution')
	saveComponentsPETimeDistribution = outputcfg.get('SaveComponentsPETimeDistribution')	
	saveTraces = outputcfg.get('SaveTraces')
	saveEnergy = outputcfg.get('SaveEnergy')
	saveComponentsEnergy = outputcfg.get('SaveComponentsEnergy')

	print("[INFO] Reading output file: %s" %ifile)
	# check if file is compressed
	if compressOutput:
		data = pd.read_json(ifile, lines=True, compression='gzip')
	else:
		data = pd.read_json(ifile, lines=True)


	detectors = []
	optdevices = []
	for name in data.columns:
		if 'Detector' in name:
			detectors.append(name)
		elif 'OptDevice' in name:
			optdevices.append(name)

	print("[INFO] Found information of Detector(s):")
	for det in detectors:
		print("%s \n" %det)
	
	print("[INFO] Found information of Optical Device(s):")
	for optdev in optdevices:
		print("%s \n" %optdev)


	
	# extract information based on output options
	if saveInput:
		print("-------------- Input Flux --------------")
		# convert this to function
		print("[INFO] Reading information of input flux")
		key = 'InputFlux'
		inputData = data[key][0] # list containing all events
		nEvents = len(inputData)
		print("Number of injected particles = %i" %nEvents)
		# preparing data holder
		inputDataDict = defaultdict(list)
		for i in range(nEvents):
			# each event is stored as a dictionary in a list of nEvents
			datai = inputData[i]
			# first element of the dictionary is the particle (CORSIKA) ID
			inputDataDict["ID"].append(datai['ID'])
			# the following three elements are the momentum components [MeV]
			px = datai['Momentum'][0]
			py = datai['Momentum'][1]
			pz = datai['Momentum'][2]
			pTot = np.sqrt(px*px + py*py + pz*pz)
			inputDataDict["px"].append(px)
			inputDataDict["py"].append(py)
			inputDataDict["pz"].append(pz)
			inputDataDict["pTot"].append(pTot)
			# the last three elements are the coordinates of the injection position [cm]
			inputDataDict["x"].append(datai['Position'][0])
			inputDataDict["y"].append(datai['Position'][1])
			inputDataDict["z"].append(datai['Position'][2])

		inputDataDf = pd.DataFrame(inputDataDict)
		ids = np.unique(inputDataDf.ID)
		# [TODO]
		# PRINT INFORMATION ABOUT MOMENTUM OF EACH PARTICLE TYPE (number of particles, histogram of momentum, mean value, etc)
		# allow plot of spectrum in options

		if True:
			# set option for plotting in parser
			fig = plt.figure()
			for iid in ids:
				plt.hist(inputDataDf[inputDataDf.ID == iid].pTot.apply(np.log10), histtype='step', lw=1.7, label='%i' %iid)

			plt.yscale('log')
			plt.xlabel(r'log(Particle Momentum / MeV)')
			plt.ylabel(r'Counts')
			plt.legend(loc='upper right', title='CORSIKA ID')
			plt.title('%s' %key)
	
	if saveEnergy:
		print("-------------- Energy Deposits --------------")
		print("[INFO] Reading information of Deposited Energy")
		key = 'EnergyDeposit'
		"""
		DepositEnergy is part of "DetectorSimData", therefore
		we have to access first to the data of each detector
		and then access to information of energy deposits in 
		that detector.
		"""
		detDataDict = defaultdict(list)
		
		for keydet in detectors:
			print("[INFO] Accessing data of Detector: %s" %keydet)
			
			eDepData = np.array(data[keydet][0]['DetectorSimData'][key])
			edep = eDepData[eDepData > 0]

			detDataDict['EnergyDeposit_%s' %keydet].append(edep)

		# set option for plotting in parser
		if True:
			fig = plt.figure()
			for keydet in detectors:
				edep = detDataDict['EnergyDeposit_%s' %keydet][0]
				bins = max(10, int(np.sqrt(len(edep))))
				plt.hist(edep, bins, histtype='step', lw=1.5, label='%s' %keydet)

			plt.yscale('log')
			plt.xlabel(r'Deposited Energy / MeV')
			plt.ylabel(r'Counts')
			plt.legend(loc='upper right')
			plt.title('%s' %key)

	if savePETimeDistribution:
		print("-------------- Signals --------------")
		print("[INFO] Reading information of Optical Device(s) signals")
		key = 'PETimeDistribution'
		
		optDataDict = defaultdict(list)
		for keyoptdev in optdevices:
			# this returns a list where each element is the PETimeDistribution of each injected particle
			peTimeDistributions = data[keyoptdev][0][key]
			# length of the list should match the number of events
			assert(len(peTimeDistributions) == nEvents)

			# compute charge as count of PE
			for peTimeDisti in peTimeDistributions:
				optDataDict['Charge_%s' %keyoptdev].append(len(peTimeDisti))

		# set option for plotting in parser
		if True:
			# plot of charge histogram
			fig = plt.figure()
			for keyoptdev in optdevices:
				charge = np.array(optDataDict['Charge_%s' %keyoptdev])
				bins = max(10, int(np.sqrt(len(charge))))
				plt.hist(charge, bins, histtype='step', lw=1.5, label='%s' %keyoptdev)

			plt.yscale('log')
			plt.xlabel(r'Number of photo-electrons')
			plt.ylabel(r'Counts')
			plt.legend(loc='upper right')
			plt.title('%s' %key)

	plt.show()
	embed()




if __name__ == "__main__":

	main()