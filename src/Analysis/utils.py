# useful functions and definitions
import numpy as np
import pandas as pd
from collections import defaultdict
import matplotlib.pyplot as plt
from IPython import embed
components = ['eElectromagnetic', 'eHadronic', 'eMuonDecay', 'eMuonic']


def GetInputFlux(data):
	"""
		Extracts information about injected particles

		Args: 
			- data: Pandas DataFrame with simulation output

		Returns: 
			- Pandas DataFrame with particle ID, momentum and position coordinates
	"""
	print("[INFO] GetInputFlux: Reading information of injected particles")
	key = 'InputFlux'
	inputData = data[key][0] # list containing all events
	n_events = len(inputData)
	print("Number of injected particles = %i" %n_events)
	inputDatadict = defaultdict(list)
	for i in range(n_events):
		# each event is stored as a dictionary in a list of all events
		datai = inputData[i]
		# first element of the dictionary is the particle (CORSIKA) id
		inputDatadict["particle_id"].append(datai['ID'])
		# the following three elements are the momentum componentes [in MeV]
		px = datai['Momentum'][0]
		py = datai['Momentum'][1]
		pz = datai['Momentum'][2]
		ptot = np.sqrt(px*px + py*py + pz*pz)
		inputDatadict["particle_momentum_x"].append(px)
		inputDatadict["particle_momentum_y"].append(py)
		inputDatadict["particle_momentum_z"].append(pz)
		inputDatadict["particle_momentum_tot"].append(ptot)
		# the last three elements are the coordinates of the injection position [ in cm]
		inputDatadict["particle_position_x"].append(datai['Position'][0])
		inputDatadict["particle_position_y"].append(datai['Position'][1])
		inputDatadict["particle_position_z"].append(datai['Position'][2])

	# convert to pandas DataFrame
	inputDataDf = pd.DataFrame(inputDatadict)
	return inputDataDf

def PlotInputFlux(df):
	"""
		Plots related to input flux:
			- Momentum distribution
			- ...
		Args: 
			- df: Pandas DataFrame from GetInputFlux()
	"""

	# plot momentum distribution
	ids = np.unique(df.particle_id)
	
	fig = plt.figure()
	sfig = fig.add_axes([0.15, 0.11, 0.845, 0.78])
	sfig.set_xlabel(r'log(p / MeV)')
	sfig.set_ylabel(r'Counts')
	sfig.set_yscale('log')
	for iid in ids:
		plt.hist(df[df.particle_id == iid].particle_momentum_tot.apply(np.log10),
			histtype='step', lw=1.7, label='%i' %iid)

	ncols = max(1, int(len(ids) / 4))
	plt.legend(loc='upper right', title='Corsika ID', ncol=ncols)
	sfig.set_title('InputFlux')
	
	return fig

def GetDepositedEnergy(data, detectors):
	
	"""
		Gets deposited energy for each detector
		Args:
			- data: Pandas DataFrame with simulation output
			- detectors: list of detector names as they were stored in the outputfile

		Returns:
			- container: dictionary with an array of values of deposited energy per detector
	"""

	key = 'EnergyDeposit'
	container = {}
	for keydet in detectors:
		print("[INFO] GetDepositedEnergy: Accessing data of Detector: %s" %keydet)
		eDepData = np.array(data[keydet][0][key])
		# skip particles that did not hit the detector
		edep = eDepData[eDepData > 0]
		container['EnergyDeposit_%s' %keydet] = edep

	# convert to Pandas DataFrame
	eDepDf = pd.DataFrame(container)
	return eDepDf

def PlotDepositedEnergy(df):
	"""
		Plots energy deposits on each detector
		Args: 
			- df: Pandas DataFrame from GetDepositedEnergy()
	"""

	for name in df.columns:
		fig = plt.figure()
		sfig = fig.add_axes([0.15, 0.11, 0.845, 0.78])
		sfig.set_xlabel(r'Deposited Energy / MeV')
		sfig.set_ylabel(r'Counts')
		sfig.set_yscale('log')

		edep = df[name].values
		bins = max(10, int(np.sqrt(len(edep))))

		plt.hist(edep, bins, histtype='step', lw=1.5, color='k')
		plt.title('%s' %name)

	return fig

def GetComponentsDepositedEnergy(data, detectors):
	"""
	
	"""
	key = 'EnergyDeposit'
	container = {}
	for keydet in detectors:
		print("[INFO] GetComponentesDepositedEnergy: Accessing data of Detector: %s" %keydet)

		depositedEnergy = data[keydet][0][key]
		component_eDep = {}
		for comp in components:
			try:
				component_eDep[comp] = depositedEnergy[comp]
			except KeyError:
				print("[WARNING] GetComponentesDepositedEnergy: Component not found in data: ", comp )
		container[keydet] = component_eDep
	# convert to Pandas DataFrame
	componentsEdepDf = pd.DataFrame(container)
	return componentsEdepDf

def PlotComponentsDepositedEnergy(df):

	for name in df.columns:
		fig = plt.figure()
		sfig = fig.add_axes([0.15, 0.11, 0.845, 0.78])
		sfig.set_xlabel(r'Deposited Energy / MeV')
		sfig.set_ylabel(r'Counts')
		sfig.set_yscale('log')

		for comp in components:
			try:
				edepcomp = df[name][comp]
				bins = max(10, int(np.sqrt(len(edepcomp))))
				plt.hist(edepcomp, bins, histtype='step', lw=1.5, label='%s' %comp)
			except KeyError:
				print("[WARNING] PlotComponentsDepositedEnergy: Component not found in data: ", comp)

		plt.legend(loc='upper right', title='Component:')
		plt.title('%s' %name)

	return fig


def GetTraces(data, optdevices):
	print("[INFO] GetTraces: Reading information of Optical Device signals")
	key = 'PETimeDistribution'
	container = {}
	for keyoptdev in optdevices:
		peTimeDistributions = data[keyoptdev][0][key]
		
		container[keyoptdev] = peTimeDistributions

	tracesDf = pd.DataFrame(container)
	return tracesDf

def PlotChargeHistogram(df):
	"""
	Plots total charge histogram (see PlotComponentsChargeHistogram())
	Args:
		- df: Pandas DataFrame with traces from GetTraces()
	"""
	
	fig = plt.figure()
	sfig = fig.add_axes([0.15, 0.11, 0.845, 0.78])
	sfig.set_yscale('log')
	sfig.set_xlabel(r'Number of photo-electrons')
	sfig.set_ylabel(r'Counts')

	for optdev in df.columns:
		optdf = df[optdev]
		charge = []
		for trace in optdf:
			charge.append(len(trace))

		bins = max(10, int(np.sqrt(len(charge))))
		plt.hist(charge, bins, histtype='step', lw=1.5, label='%s' %optdev)

	plt.legend(loc='upper right')
	return fig


def GetComponentsTraces(data, optdevices):
	"""
		
	"""
	print("[INFO] GetComponentTraces: Reading information of Optical Device signals")
	key = 'PETimeDistribution'
	container = {}
	for keyoptdev in optdevices:
		# this returns a dictionary where each key contains the PETimeDistributions of the corresponding particle component
		peTimeDistributions = data[keyoptdev][0][key]
		component_traces = {}
		for comp in components:
			try:
				component_traces[comp] = peTimeDistributions[comp]
			except KeyError:
				print("[WARNING] GetComponentTraces: Component not found in data: ", comp )
		container[keyoptdev] = component_traces

	# convert to Pandas DataFrame
	componentTracesDf = pd.DataFrame(container)
	return componentTracesDf


def PlotComponentsChargeHistogram(df):
	"""
		Plots charge histogram by components
		Args:
			- df: Pandas DataFrame from GetComponentsTraces()
	"""

	# first loop over optical devices in the data frame
	for optdev in df.columns:

		# accessing dataframe of that optical device
		optdf = df[optdev]
		# container for each component
		chargedict = defaultdict(list)
		
		fig = plt.figure()
		sfig = fig.add_axes([0.15, 0.11, 0.845, 0.78])
		sfig.set_yscale('log')
		sfig.set_xlabel(r'Number of photo-electrons')
		sfig.set_ylabel(r'Counts')

		# access to component traces
		for comp in components:
			try:
				ctraces = optdf[comp]
				# loop over traces
				for trace in ctraces:
					# charge = sum(PE) = elements of the array
					chargedict[comp].append(len(trace))
			
				# plot histogram
				plt.hist(chargedict[comp], bins=int(np.sqrt(len(chargedict[comp]))), histtype='step', lw=1.5, label='%s' %comp)
			except KeyError:
				print("[WARNING] PlotComponentsChargeHistogram: Component not found in data: ", comp)
		plt.legend(loc='upper right', title='Component:')
		plt.title(r'Charge histogram of %s' %optdev)
		return fig

