#!/usr/bin/python3
# Python code to read Meiga output
# author: alvaro taboada
# date: march 2023
import json
import gzip
import argparse
#import numpy as np
#import pandas as pd
import pickle

import os
from glob import glob

from utils import *

import matplotlib.pyplot as plt
# console for debugging
from IPython import embed

def completepath(s, cfiledir):
	if s[0] == '/':
		return s
	else:
		return os.path.join(cfiledir, s)

def Process(options):

	"""
	Description of the main function.

	This script is not intended for analysis.
	It is intended to be used as a guide
	for reading output files of the simulation.

	Inputs:
		- configuration file (Cfg.json)
	Returns:
		Statistics and plots of charge histograms / energy deposits

	"""
	cfgfile = options.cfg

	# check that ifile and cfg exists
	
	if not (os.path.exists(cfgfile)):
		print("[ERROR] Process: Configuration file %s does not exist. Wrong path or misspelled filename." %cfgfile)
		return

	print("[INFO] Process: Reading configuration file: %s" %cfgfile)
	cfgfile2 = open(cfgfile, "rt")
	cfgdata = json.load(cfgfile2)
	outputcfg = cfgdata['Output']
	cfgfiledir = os.path.dirname(cfgfile)
	# reading application output options from configuration file
	compressOutput = outputcfg.get('CompressOutput')
	ifile = completepath(outputcfg.get('OutputFile'), cfgfiledir)
	if compressOutput:
		ifile += '.gz'

	if not (os.path.exists(ifile)):
		print("[ERROR] Process: Input file %s does not exist. Wrong path or misspelled filename." %ifile)
		return
	

	saveInput = outputcfg.get('SaveInput')
	savePETimeDistribution = outputcfg.get('SavePETimeDistribution')
	saveComponentsPETimeDistribution = outputcfg.get('SaveComponentsPETimeDistribution')	
	#saveTraces = outputcfg.get('SaveTraces')
	saveEnergy = outputcfg.get('SaveEnergy')
	saveComponentsEnergy = outputcfg.get('SaveComponentsEnergy')

	# redundant if saveComponents* set to True
	if saveComponentsPETimeDistribution:
		savePETimeDistribution = False
	if saveComponentsEnergy:
		saveEnergy = False

	processedcfg = {"InputFlux" : saveInput,
									"PETimeDistribution" : savePETimeDistribution,
									"ComponentsPETimeDistribution" : saveComponentsPETimeDistribution,
									"DepositedEnergy" : saveEnergy,
									"ComponentsDepositedEnergy" : saveComponentsEnergy}

	print("\n")
	print("[INFO] Process: Reading output file: %s" %ifile)
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

	print("[INFO] Process: Found information of Detector(s):")
	for det in detectors:
		print("%s \n" %det)
	
	print("[INFO] Process: Found information of Optical Device(s):")
	for optdev in optdevices:
		print("%s \n" %optdev)


	###########################################################################
	# Data is extracted according to Output settings in configuration file
	###########################################################################
	processedData = {}

	if saveInput:
		processedData['InputFlux'] = GetInputFlux(data)
	if saveEnergy:
		processedData['DepositedEnergy'] = GetDepositedEnergy(data, detectors)
	if saveComponentsEnergy:
		processedData['ComponentsDepositedEnergy'] = GetComponentsDepositedEnergy(data, detectors)
	if saveComponentsPETimeDistribution:
		processedData['ComponentsPETimeDistribution'] = GetComponentsTraces(data, optdevices)
	if savePETimeDistribution:
		processedData['PETimeDistribution'] = GetTraces(data, optdevices)
	
	return processedData, processedcfg


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="This script reads output files from simulations with Meiga.", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	# parser.add_argument("-i", 
	# 										dest="ifile", 
	# 										help="full path of the Application's output file. Exaple: /home/user/meiga/Application/output.json.gz", 
	# 										default=None, required=True)
	parser.add_argument("-c",
											dest="cfg",
											help="full path of the Application's configuration file. Example: /home/user/meiga/Application/G4Application.json",
											default=None, required=True)

	options = parser.parse_args()

	# main function: reads the simulation output file
	# and returns a container with all requested information
	processedData, processedcfg = Process(options)
	
	#########################################################################
	# PLOTS (section under development)
	#########################################################################
	
	plotcfg = {"InputFlux" : PlotInputFlux,
						 "DepositedEnergy" :  PlotDepositedEnergy,
						 "ComponentsDepositedEnergy" : PlotComponentsDepositedEnergy,
						 "PETimeDistribution" : PlotChargeHistogram,
						 "ComponentsPETimeDistribution" : PlotComponentsChargeHistogram
						}

	plots = 0
	for option in processedcfg.keys():
		if processedcfg.get(option):
			data = processedData[option]
			plotcfg.get(option)(data)
			print("Plotting: ", option)
			plots += 1
	
	if plots:
		plt.show()
		# :D
	else:
		print("Nothing to plot!")

	# ------------------------------------------------------------------------