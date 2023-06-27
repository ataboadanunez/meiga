# useful functions and definitions
import numpy as np
import pandas as pd
from collections import defaultdict
import matplotlib.pyplot as plt
from IPython import embed
components = ['eElectromagnetic', 'eHadronic', 'eMuonDecay', 'eMuonic']
components_id = {
									'eElectromagnetic' : [1, 2, 3],
									'eMuonic'          : [5, 6, 95, 96],
									'eHadronic'        : [7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 71, 72, 73, 74],
								}

def IdToComponent(iid):
	for comp in components_id.keys():
		if iid in components_id.get(comp):
			return comp
	
	return 'eUnknown'

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
		inputDatadict["event_id"].append(i)
		# first element of the dictionary is the particle (CORSIKA) id
		inputDatadict["particle_id"].append(datai['ID'])
		inputDatadict["component"].append(IdToComponent(datai['ID']))
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
		edep = eDepData[eDepData >= 0]
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

		plt.hist(edep[edep>0], bins, histtype='step', lw=1.5, color='k')
		name = ''.join([s+' ' for s in name.split('_')])
		plt.title(r'%s' %name)

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
		name = ''.join([s+' ' for s in name.split('_')])
		plt.title(r'%s' %name)

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
		# remove _ from name for labeling 
		optdev_name = ''.join([s+' ' for s in optdev.split('_')])
		plt.hist(charge, bins, histtype='step', lw=1.5, label='%s' %optdev_name)

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
		# remove _ from name for labeling 
		optdev_name = ''.join([s+' ' for s in optdev.split('_')])
		plt.title(r'Charge histogram of %s' %optdev_name)
		return fig


def MergeInputOutput(processedData, processedCfg):
	
	# Fix Iterator over Detectors

	optdevices = processedData['OptDevice_ID']
	detectors  = processedData['Detector_ID']

	particle_df = pd.DataFrame()
	saveInput = processedCfg['InputFlux']
	saveComponentsEnergy = processedCfg['ComponentsDepositedEnergy']
	saveEnergy = processedCfg['DepositedEnergy']
	saveComponentsPETimeDistribution = processedCfg['ComponentsPETimeDistribution']
	savePETimeDistribution = processedCfg['PETimeDistribution']

	# build particle data
	if saveInput and (saveComponentsEnergy or saveEnergy) and (saveComponentsPETimeDistribution or savePETimeDistribution):
		
		# saveInput option is needed to track particle type

		# get input dataframe
		input_df = processedData['InputFlux']

		# merge EnergyDeposit
		if saveEnergy:

			edep_df = processedData['DepositedEnergy']
			
			assert len(edep_df) == len(input_df)

			input_df_c = input_df.copy()
			# merge energy deposited data with input data
			input_df_c['DepositedEnergy'] = edep_df

		elif saveComponentsEnergy:
			# separate input data per components
			muon_df = input_df[input_df.component == 'eMuonic']
			em_df = input_df[input_df.component == 'eElectromagnetic']
			had_df = input_df[input_df.component == 'eHadronic']

			muon_df_c = muon_df.copy()
			em_df_c = em_df.copy()
			had_df_c = had_df.copy()
			
			muon_df_c['DepositedEnergy'] = processedData['ComponentsDepositedEnergy']['Detector_0']['eMuonic']
			em_df_c['DepositedEnergy'] = processedData['ComponentsDepositedEnergy']['Detector_0']['eElectromagnetic']
			had_df_c['DepositedEnergy'] = processedData['ComponentsDepositedEnergy']['Detector_0']['eHadronic']

			input_df_c = pd.concat([muon_df_c, em_df_c, had_df_c])

		# merge PETimeDistribution 
		if savePETimeDistribution:
			
			input_df_c2 = input_df_c.copy()

			# loop over opt. devices
			for odid, optdev in enumerate(optdevices):
				input_df_c2['PE_%i' %odid] = processedData['PETimeDistribution'][optdev]

			particle_df = input_df_c2

		elif saveComponentsPETimeDistribution:
			
			# separate input data per components
			muon_df = input_df_c[input_df_c.component == 'eMuonic']
			em_df = input_df_c[input_df_c.component == 'eElectromagnetic']
			had_df = input_df_c[input_df_c.component == 'eHadronic']

			muon_df_c = muon_df.copy()
			em_df_c = em_df.copy()
			had_df_c = had_df.copy()

			for odid, optdev in enumerate(optdevices):
				pe_ = processedData['ComponentsPETimeDistribution'][optdev]
				muon_df_c['PE_%i' %odid] = pe_['eMuonic']
				em_df_c['PE_%i' %odid] = pe_['eElectromagnetic']
				had_df_c['PE_%i' %odid] = pe_['eHadronic']

			particle_df = pd.concat([muon_df_c, em_df_c, had_df_c])

			# sort dataframe by event_id to keep order
			particle_df = particle_df.sort_values('event_id')
			particle_df = particle_df.set_index('event_id', drop=False)

	return particle_df


def GetMuonDecaySignals(processedData, merged_df):

	"""
		Returns DataFrame with PE time distributions
		produced by decayed muons.
				
	"""

	optdevices = processedData['OptDevice_ID']
	# mask muons in merged DataFrame
	muon_ids = merged_df[merged_df.component == 'eMuonic'].event_id
	
	# create container for muon decay signals
	muonDecaySignals = defaultdict(list)

	for evid in muon_ids:
		muonDecaySignals['parent_id'].append(evid)

		for odid, optdev in enumerate(optdevices):
			# get muon decay traces
			muonDecayTraces = processedData['ComponentsPETimeDistribution'][optdev]['eMuonDecay']
			assert len(muonDecayTraces) == len(merged_df)
		
			muonDecaySignals['PE_%i' %odid].append(muonDecayTraces[evid])

	muonDecaySignals_df = pd.DataFrame(muonDecaySignals)

	return muonDecaySignals_df