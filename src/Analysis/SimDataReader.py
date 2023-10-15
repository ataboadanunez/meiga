# Python Class to read SimData 
# Author: Alvaro Taboada
# Date: June 2023

import os
import json
import gzip
import numpy as np
import pandas as pd
from collections import defaultdict

def completepath(s, cfiledir):
	if s[0] == '/':
		return s
	else:
		return os.path.join(cfiledir, s)


class SimDataReader:

	"""
	Reads JSON output files from Meiga simulations.

	"""

	class Config:
		def __init__(self, cfg_file_path):

			with open(cfg_file_path) as file:
				config_data = json.load(file)

			cfgfiledir = os.path.dirname(cfg_file_path)

			# output file configuration parameters
			output_config = config_data['Output']
			self.compress_output = output_config.get('CompressOutput')
			self.output_file = completepath(output_config.get('OutputFile'), cfgfiledir)

			# add gzip extenssion to filename 
			if self.compress_output:
				self.output_file += '.gz'

			# general output parameters
			self.save_input = output_config.get('SaveInput')
			self.save_petime = output_config.get('SavePETimeDistribution')
			self.save_petime_comp = output_config.get('SaveComponentsPETimeDistribution')
			self.save_energy = output_config.get('SaveEnergy')
			self.save_energy_comp = output_config.get('SaveComponentsEnergy')

	def __init__(self, output_file_path : str):

		# # First make an instance of Config
		# self._config = self.Config(cfg_file_path)
		self._output_file = output_file_path
		
		# class objects are filled with the `set` methods
		self._data = None
		self._detector_list = None
		self._event_list = []
		self._input_flux = None

		# This method is called when we initialize the class
		self.load_data()
		


	def load_data(self):

		"""

		Loads data from output file and the detector list

		"""

		ext = os.path.splitext(self._output_file)[-1]
		
		if ext == '.gz':
			with gzip.open(self._output_file, 'r') as f:
				filedata = json.load(f)
				self._data = filedata.get('Output')
				self._detector_list = filedata.get('DetectorList')
			f.close()
		else:
			with open(self._output_file, 'r') as f:
				filedata = json.load(f)
				self._data = filedata.get('Output')
				self._detector_list = filedata.get('DetectorList')
			f.close()


		# load event list
		for key in self._data.keys():
			if 'Event_' in key:
				
				# keep only ID from 'Event_ID'
				self._event_list.append(int(key.split('_')[-1]))

		self._event_list = np.array(self._event_list)

		if self._data is None:
			print('[WARNING] Unable to get data from file! Check file content.')
		else:
			print('[INFO] load_data(): Data loaded successfully!')

	def get_data(self):
		return self._data

	def get_event_list(self):
		"""
		
		Returns list(int) sorted event IDs

		"""

		return self._event_list

	def get_event(self, evid : int):

		"""
		Args:
			- evid (int): id of the event
		
		Returns dict with event data 
		
		"""

		if self._data is None:
			self.load_data()

		# construct event key from event id
		event_key = 'Event_'+str(evid)

		if self._data.get(event_key) is not None:
			return self._data.get(event_key)
		else:
			print('[WARNING] get_event(): Unknown event with ID', evid)
			return None

	
	def get_input_flux(self) -> pd.DataFrame:
		
		"""
		Returns Pandas DataFrame with information about input particles
		"""

		if self._data is None:
			self.load_data()

		evids = self.get_event_list()
		inpdict = defaultdict(list)
		key = 'InputFlux'

		# loop over events
		for evid in evids:
			# access to each event to extract information
			event = self.get_event(evid)
			inp = event.get('InputFlux')

			if inp is not None:
				# particle ID is the PDG Encoding
				# TODO: add particle component / CORSIKA ID
				inpdict['id'].append(inp['ID'])
				# particle momentum components
				inpdict['px'].append(inp['Momentum'][0])
				inpdict['py'].append(inp['Momentum'][1])
				inpdict['pz'].append(inp['Momentum'][2])
				# particle injection position
				inpdict['x'].append(inp['Position'][0])
				inpdict['y'].append(inp['Position'][1])
				inpdict['z'].append(inp['Position'][2])
				
		return pd.DataFrame(inpdict)


	def get_detector_list(self):

		"""
		Returns DataFrame with detector information
		"""

		return pd.DataFrame(self._detector_list)

	def GetDetectorSimData(self, det_id : int):
		"""
		Returns pointer to the DetectorSimData given its id
		"""
		return self.DetectorSimData(det_id, self._data)

	class DetectorSimData:
			
		"""
		Class to store data at the Detector level
		(e.g. energy deposits, particle counters)

		It also contains a method to access stored
		from Optical Devices (OptDeviceSimData)

		"""

		def __init__(self, det_id : int, data : dict):

			self._detector_id = det_id
			self._parent_simdata = data

			self._deposited_energy = None
			self._binary_counter = None

		
		def get_deposited_energy(self) -> np.array:
			
			"""
			Returns numpy array (float) of deposited energy in the detector
			"""
			detectorkey = 'Detector_' + str(self._detector_id)
			edep_list = []
			# loop over events and extract deposited energy
			for eventkey in self._parent_simdata.keys():
				# get detector level
				detector_data = self._parent_simdata[eventkey].get(detectorkey)

				if detector_data is not None:
					edep = detector_data.get("EnergyDeposit")
					if edep is not None:
						edep_list.append(edep)
				
				else:
					continue
			
			self._deposited_energy = np.array(edep_list)
			
			return self._deposited_energy

		def get_binary_counter(self) -> np.array:
			
			"""
			
			Returns numpy array (str) with binary counters in a scintillator detector
			The binary counter is a string of `1` and `0` in the indices of the bars
			that were hit or not, respectively.
			Example: a binary counter of `010000001000` indicates a scintillator with 
			12 bars where the bars 2 and 9 were hit by a primary particle.

			"""
			
			detectorkey = 'Detector_' + str(self._detector_id)
			binary_list = []

			# loop over events and extract binary counters
			for eventkey in self._parent_simdata.keys():
				# go to detector data
				detector_data = self._parent_simdata[eventkey].get(detectorkey)

				if detector_data is not None:
					counter = detector_data.get("BinaryCounter")
					if counter is not None:
						binary_list.append(counter)

				else:
					continue

			return np.array(binary_list)

		def GetOptDeviceSimData(self, od_id : int):
			"""
			Returns pointer to OptDeviceSimData object
			"""
			return self.OptDeviceSimData(od_id, self._detector_id, self._parent_simdata)

		class OptDeviceSimData:
			"""
			Class to store information at Optical Device level
			(charge, photo-electron time distributions, etc)
			"""

			def __init__(self, od_id : int, det_id : int, data : dict):

				self._od_id = od_id
				self._det_id = det_id
				self._parent_simdata = data

				self._charge = None
				self._pe_time_dist = None

			def get_charge(self) -> np.array:
				"""
				Returns numpy array (int) with charge (number of photo-electrons) 
				collected by the optical device on each event
				"""
				detectorkey = 'Detector_' + str(self._det_id)
				optdevicekey = 'OptDevice_' + str(self._od_id)
				charge_list = []

				for eventkey in self._parent_simdata.keys():

					detector_data = self._parent_simdata[eventkey].get(detectorkey)

					if detector_data is None:
						print('[WARNING] get_charge(): There is no data from detector ', detectorkey)
						return

					od_data = detector_data.get(optdevicekey)

					if od_data is None:
						print('[WARNING] get_charge(): There is no data from opt device ', optdevicekey, " in detector ", detectorkey)
						return

					charge = od_data.get("Charge")
					if charge is not None:
						charge_list.append(charge)
					else:
						continue

				return np.array(charge_list)

			def get_pe_time_distribution(self) -> list:
				"""
				Returns list (list) with arrival times of
				photo-electrons to the optical device.

				Each event contains a list of arrival times.
				"""

				detectorkey = 'Detector_' + str(self._det_id)
				optdevicekey = 'OptDevice_' + str(self._od_id)
				pe_time_list = []


				for eventkey in self._parent_simdata.keys():

					detector_data = self._parent_simdata[eventkey].get(detectorkey)

					if detector_data is None:
						print('[WARNING] get_charge(): There is no data from detector ', detectorkey)
						return

					od_data = detector_data.get(optdevicekey)

					if od_data is None:
						print('[WARNING] get_charge(): There is no data from opt device ', optdevicekey, " in detector ", detectorkey)
						return

					pe_time = od_data.get("PETimeDistribution")

					if pe_time is not None:
						pe_time_list.append(pe_time)
					else:
						pe_time_list.append([])
				
				return pe_time_list

			def get_pe_time_distribution(self, component=None) -> list:
				"""
				Returns list (list) with PE time distribution
				by particle component:

				Components: 'eMuonic', 'eElectromagnetic', 'eHadronic', 'eMuonDecay'

				Each event contains a list of arrival times.
				"""

				detectorkey = 'Detector_' + str(self._det_id)
				optdevicekey = 'OptDevice_' + str(self._od_id)
				pe_time_list = []


				for eventkey in self._parent_simdata.keys():

					detector_data = self._parent_simdata[eventkey].get(detectorkey)

					if detector_data is None:
						print('[WARNING] get_charge(): There is no data from detector ', detectorkey)
						return

					od_data = detector_data.get(optdevicekey)

					if od_data is None:
						print('[WARNING] get_charge(): There is no data from opt device ', optdevicekey, " in detector ", detectorkey)
						return

					pe_time = od_data.get("PETimeDistribution_%s" %component)

					if pe_time is not None:
						pe_time_list.append(pe_time)
					else:
						pe_time_list.append([])
				
				return pe_time_list