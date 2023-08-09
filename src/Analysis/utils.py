# useful functions and definitions
import numpy as np
import pandas as pd
from collections import defaultdict
import matplotlib.pyplot as plt
from IPython import embed

# definitions
kNucleusBase = 1000000000
kNucleusMax = 1000100300
kAtomicNumberFactor = 1
kAtomicNumberMask = 1000
kChargeFactor = 1000
kChargeMask = 1000000

components = ['eElectromagnetic', 'eHadronic', 'eMuonDecay', 'eMuonic']

class ParticleConverter:
    def __init__(self):
        # Dictionary to store the mappings between CORSIKA ID and PDG Encoding
        self.corsika_to_pdg = {}
        self.pdg_to_corsika = {}
        self.components_id  = {}

        # Register particles when class is initialized
        self.register_particle(1, 22, 'eElectromagnetic')
        self.register_particle(2, -11, 'eElectromagnetic')
        self.register_particle(3, 11, 'eElectromagnetic')

        self.register_particle(5, -13, 'eMuonic')
        self.register_particle(6, 13, 'eMuonic')
        self.register_particle(7, 111, 'eHadronic')
        self.register_particle(8, 211, 'eHadronic')
        self.register_particle(9, -211, 'eHadronic')

        self.register_particle(10, 130, 'eHadronic')
        self.register_particle(11, 321, 'eHadronic')
        self.register_particle(12, -321, 'eHadronic')
        self.register_particle(13, 2112, 'eHadronic')
        self.register_particle(14, 2212, 'eHadronic')
        self.register_particle(15, -2212, 'eHadronic')
        self.register_particle(16, 310, 'eHadronic')
        self.register_particle(17, 221, 'eHadronic')
        self.register_particle(18, 3122, 'eHadronic')
        self.register_particle(19, 3222, 'eHadronic')
        self.register_particle(20, 3212, 'eHadronic')
        self.register_particle(21, 3112, 'eHadronic')
        self.register_particle(22, 3322, 'eHadronic')
        self.register_particle(23, 3312, 'eHadronic')
        self.register_particle(24, 3332, 'eHadronic')
        self.register_particle(25, -2112, 'eHadronic')
        self.register_particle(26, -3122, 'eHadronic')
        self.register_particle(27, -3222, 'eHadronic')
        self.register_particle(28, -3212, 'eHadronic')
        self.register_particle(29, -3112, 'eHadronic')
        self.register_particle(30, -3322, 'eHadronic')
        self.register_particle(31, -312, 'eHadronic')
        self.register_particle(32, -3332, 'eHadronic')

        self.register_particle(49, 333, 'eHadronic')

        self.register_particle(50, 223, 'eHadronic')
        self.register_particle(51, 113, 'eHadronic')
        self.register_particle(52, 213, 'eHadronic')
        self.register_particle(53, -213, 'eHadronic')
        self.register_particle(54, 2224, 'eHadronic')
        self.register_particle(55, 2214, 'eHadronic')
        self.register_particle(56, 2114, 'eHadronic')
        self.register_particle(57, 1114, 'eHadronic')
        self.register_particle(58, -2224, 'eHadronic')
        self.register_particle(59, -2214, 'eHadronic')
        self.register_particle(60, -2114, 'eHadronic')
        self.register_particle(61, -1114, 'eHadronic')
        self.register_particle(62, 313, 'eHadronic')
        self.register_particle(63, 323, 'eHadronic')
        self.register_particle(64, -323, 'eHadronic')
        self.register_particle(65, -313, 'eHadronic')
        self.register_particle(66, 12, 'eHadronic')
        self.register_particle(67, -12, 'eHadronic')
        self.register_particle(68, 14, 'eHadronic')
        self.register_particle(69, -14, 'eHadronic')
        
        self.register_particle(71, 221, 'eHadronic')
        self.register_particle(72, 221, 'eHadronic')
        self.register_particle(73, 221, 'eHadronic')
        self.register_particle(74, 221, 'eHadronic')

        self.register_particle(95, 95, 'eMuonic')
        self.register_particle(96, 96, 'eMuonic')

    def is_nucleus(self, pdg_encoding):
      return (kNucleusBase < pdg_encoding) and (pdg_encoding < kNucleusMax)

    def register_particle(self, corsika_id, pdg_encoding, component):
        """
        Register a particle with its CORSIKA ID, PDG Encoding and component
        """
        self.corsika_to_pdg[corsika_id] = pdg_encoding
        self.pdg_to_corsika[pdg_encoding] = corsika_id
        self.components_id[pdg_encoding] = component

    def corsika_to_pdg_encoding(self, corsika_id):
        """
        Convert CORSIKA ID to PDG Encoding.
        If the CORSIKA ID is not registered, return None.
        """
        return self.corsika_to_pdg.get(corsika_id)

    def pdg_encoding_to_corsika(self, pdg_encoding):
        """
        Convert PDG Encoding to CORSIKA ID.
        If the PDG Encoding is not registered, return None.
        """

        # check if particle is a nucleus
        if self.is_nucleus(pdg_encoding):
          # extract Z and A from PDG Encoding
          z = (pdg_encoding / kChargeFactor) % kChargeMask
          a = (pdg_encoding / kAtomicNumberFactor) % kAtomicNumberMask

          return a * 100 + z

        else:

          return self.pdg_to_corsika.get(pdg_encoding)

    def get_component(self, pdg_encoding):

      if self.is_nucleus(pdg_encoding):
        return 'eHadronic'

      else:

        component_name = self.components_id.get(pdg_encoding)
        if component_name is None:
          return 'eUnknown'
        else:
         return component_name

################################################################
# Functions to access data (to be part of a class SimDataReader)
################################################################

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
  inputData = data[key] # list containing all events
  n_events = len(inputData)
  print("Number of injected particles = %i" %n_events)
  inputDatadict = defaultdict(list)
  # initalize ParticleConverter class
  converter = ParticleConverter()
  for i in range(n_events):
    # each event is stored as a dictionary in a list of all events
    datai = inputData[i]
    inputDatadict["event_id"].append(i)
    # first element of the dictionary is the particle id (PDG Encoding)
    pdg_id = datai['ID']
    corsika_id = converter.pdg_encoding_to_corsika(pdg_id)
    inputDatadict["particle_id"].append(corsika_id)
    inputDatadict["component"].append(converter.get_component(pdg_id))
    #inputDatadict["component"].append(IdToComponent(corsika_id))
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
    eDepData = np.array(data[keydet][key])
    # skip particles that did not hit the detector
    edep = eDepData[eDepData >= 0]
    container[keydet] = edep

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
    
    depositedEnergy = data[keydet][key]
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


def GetTraces(data, detectors):
  print("[INFO] GetTraces: Reading information of Optical Device signals")
  keydata = 'PETimeDistribution'
  container = {}

  for keydet in detectors:
    for name in data[keydet].keys():
      if 'OptDevice_' in name:
        keyoptdev = keydet + '/' + name
        peTimeDistributions = data[keydet][name][keydata]
        container[keyoptdev] = peTimeDistributions

  # for keyoptdev in optdevices:
  #   peTimeDistributions = data[keyoptdev][0][key]
    
  #   container[keyoptdev] = peTimeDistributions

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


def GetComponentsTraces(data, detectors):
  """
    
  """
  print("[INFO] GetComponentTraces: Reading information of Optical Device signals")
  keydata = 'PETimeDistribution'
  container = {}

  for keydet in detectors:
    for name in data[keydet].keys():
      if 'OptDevice_' in name:
        keyoptdev = keydet + '/' + name
        peTimeDistributions = data[keydet][name][keydata]

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

      input_df_c = input_df.copy()

      # merge energy deposited data with input data
      for detector in detectors:
        input_df_c['DepositedEnergy_%s' %detector] = processedData['DepositedEnergy'][detector]

    elif saveComponentsEnergy:
      # separate input data per components
      muon_df = input_df[input_df.component == 'eMuonic']
      em_df = input_df[input_df.component == 'eElectromagnetic']
      had_df = input_df[input_df.component == 'eHadronic']

      muon_df_c = muon_df.copy()
      em_df_c = em_df.copy()
      had_df_c = had_df.copy()
      
      for detector in detectors:
        muon_df_c['DepositedEnergy_%s' %detector] = processedData['ComponentsDepositedEnergy'][detector]['eMuonic']
        em_df_c['DepositedEnergy_%s' %detector] = processedData['ComponentsDepositedEnergy'][detector]['eElectromagnetic']
        had_df_c['DepositedEnergy_%s' %detector] = processedData['ComponentsDepositedEnergy'][detector]['eHadronic']

      input_df_c = pd.concat([muon_df_c, em_df_c, had_df_c])

    # merge PETimeDistribution 
    if savePETimeDistribution:
      
      input_df_c2 = input_df_c.copy()

      # loop over opt. devices
      for odid, optdev in enumerate(processedData['PETimeDistribution'].keys()):
        input_df_c2['PE_%s' %optdev] = processedData['PETimeDistribution'][optdev]

      particle_df = input_df_c2

    elif saveComponentsPETimeDistribution:
      
      # separate input data per components
      muon_df = input_df_c[input_df_c.component == 'eMuonic']
      em_df = input_df_c[input_df_c.component == 'eElectromagnetic']
      had_df = input_df_c[input_df_c.component == 'eHadronic']

      muon_df_c = muon_df.copy()
      em_df_c = em_df.copy()
      had_df_c = had_df.copy()

      for odid, optdev in enumerate(processedData['ComponentsPETimeDistribution'].keys()):
        pe_ = processedData['ComponentsPETimeDistribution'][optdev]
        muon_df_c['PE_%s' %optdev] = pe_['eMuonic']
        em_df_c['PE_%s' %optdev] = pe_['eElectromagnetic']
        had_df_c['PE_%s' %optdev] = pe_['eHadronic']

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

  optdevices = processedData['ComponentsPETimeDistribution'].keys()
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
    
      muonDecaySignals['PE_%s' %optdev].append(muonDecayTraces[evid])

  muonDecaySignals_df = pd.DataFrame(muonDecaySignals)

  return muonDecaySignals_df