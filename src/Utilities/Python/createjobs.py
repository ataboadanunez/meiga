#!/usr/bin/env python3

# description
# author
# date

import argparse
import random
import re
import string
import json 

from glob import glob
import numpy as np
import os
from os import path, mkdir, makedirs, getcwd, chmod, listdir, environ, popen, system
from sys import stdout, argv

import tempfile
import shutil

from datetime import datetime

from IPython import embed


def abspath(s):
	if s[0] == '/':
		return s
	cwd = environ["PWD"]
	return path.join(cwd, s)

def id_generator(dict_var, Id):
	for k, v in dict_var.items():
		if k == Id:
			yield v
		elif isinstance(v, dict):
			for id_val in id_generator(v, Id):
				yield id_val

def SplitInputFile(ifile, N=100000):
	# check that input file exits
	if not path.exists(ifile):
		print("[ERROR] Input file not exists!")
		return

	print("[INFO] Splitting input file: %s" %ifile)
	print("Maximum %i lines (particles) per file" %N)
	# creating temp directory for split files
	splits_dir = tempfile.mkdtemp(prefix='flux_files-')
	print("[INFO] Destination: %s" %splits_dir)

	# split input file name
	# prefix is used to identify the input file inside the destination (job) directory
	#prefix = path.splitext(ifile)[0].split('/')[-1]
	prefix = 'input'
	print("[INFO] Using file PREFIX = %s" %prefix)
	output = path.join(splits_dir, prefix+'_')
	system("split -d -l %i %s %s" %(N, ifile, output))
	print("[INFO] Splitting completed!")

	return splits_dir


def CreateJobsNPart(base_dir, job_dir, n_part, n_max=100000):
	# copies executable and .json / .xml files from base_dir to each job_dir/jobid
	# replaces "InputNParticles" target in .json with corresponding n_part / n_max

	# returns file "joblists.txt" with list of job_dir/jobid 

	if not path.exists(base_dir):
		print("[ERROR] Base directory does not exist. Exiting ...")
		return

	if not n_part:
		raise Exception("No input particles!")

	if n_part < n_max:
		njobs = 1
	else:
		njobs = int(n_part / n_max)

	print("[INFO] Creating %i jobs" %njobs)
	jobDirs = []

	for jobid in range(njobs):
		#outfile = 'output_'+jobid+'.dat'
		jobpath = path.join(job_dir, 'run_'+str(jobid))
		if not path.exists(jobpath):
			print("[INFO] Creating job directory: %s " %jobpath)
			makedirs(jobpath)
			# copy executable and cfg files from base directory
			system("cp -r %s/* %s" %(base_dir, jobpath))

			jobDirs.append(jobpath)
		else:
			raise Exception("[ERROR] jobpath %s already exists!" %jobpath)
	
	# write log with jobs information
	jobCreationTime = datetime.now()
	jobCreationUser = environ.get("USER")
	jobLogName = path.join(job_dir, 'jobs.log')
	with open(jobLogName, "w") as f:
		f.write("# user: %s \n" %jobCreationUser)
		f.write("# date: %s \n" %jobCreationTime.strftime("%d-%b-%Y (%H:%M:%S.%f)"))

		f.write("# created jobs: \n")
		for jobi in jobDirs:
			f.write(jobi + "\n")

	f.close()

	return jobDirs


def CreateJobs(base_dir, splits_dir, job_dir):
	
	# copies executable and .json / .xml files from base_dir to each job_dir/jobid
	# replaces "inputfile" target in .json with corresponding split file

	# returns file "joblists.txt" with list of job_dir/jobid 


	# check if base_dir exists
	if not path.exists(base_dir):
		print("[ERROR] Base directory does not exist. Exiting ...")
		return

	# create one job directory per split file (input file)
	inputFiles = sorted(glob(path.join(splits_dir, '*[0-9]')))

	if len(inputFiles) == 0:
		raise Exception("No input files!")

	jobDirs = []

	for iFile in inputFiles:
		# job id is a str
		jobid = iFile.split('_')[-1]
		outfile = 'output_'+jobid+'.dat'
		# create job directory
		jobpath = path.join(job_dir, 'run_'+jobid)
		if not path.exists(jobpath):
			print("[INFO] Creating job directory: %s" %jobpath)
			makedirs(jobpath)
			# copy input file to the job directory
			system("cp -r %s %s" %(iFile, jobpath))
			# copy executable and cfg files from base directory			
			system("cp -r %s/* %s" %(base_dir, jobpath))

			jobDirs.append(jobpath)

		else:
			raise Exception("[ERROR] jobpath %s already exists!" %jobpath)


	# write log with jobs information
	jobCreationTime = datetime.now()
	jobCreationUser = environ.get("USER")
	jobLogName = path.join(job_dir, 'jobs.log')
	with open(jobLogName, "w") as f:
		f.write("# user: %s \n" %jobCreationUser)
		f.write("# date: %s \n" %jobCreationTime.strftime("%d-%b-%Y (%H:%M:%S.%f)"))

		f.write("# created jobs: \n")
		for jobi in jobDirs:
			f.write(jobi + "\n")

	f.close()

	shutil.rmtree(splits_dir)
	print("[INFO] Deleting temporary directory %s" %splits_dir)
	
	return jobDirs


def PrepareConfigFile(job_dir, nlines):

	print("[INFO] Preparing Configuration files")
	def is_exe(filename):
		return os.access(filename, os.X_OK)

	for jobpath in job_dir:

		# list elements of that directory
		# find executable, config file (.json) and input file  
		
		# this is ugly.
		cfgFileName = ''
		exeName = ''
		inputFileName = ''
		outputFileName = ''
		
		# jobid from job path
		jobid = jobpath.split('/')[-1].split('_')[-1]

		for item in listdir(jobpath):
			filename = path.join(jobpath, item)
			split_filename = path.splitext(filename)
			
			name = split_filename[0].split('/')[-1].split('_')[0]
			ext = split_filename[1]

			# this is ugly as well.
			if ext == '.json':
				cfgFileName = filename
			
			if ext == '' and is_exe(filename):
				exeName = name
			
			if not path.exists(path.join(jobpath,exeName)):
				raise Exception("[ERROR] Executable not found at %s" %jobpath)

			if name == 'input':
				#jobid = split_filename[0].split('/')[-1].split('_')[1]
				inputFileName = name + '_'+ jobid
		
		# name of file to store the simulation output
		outputFileName = 'output' + '_' + jobid + '.dat'
			
		infile 	= path.join(jobpath, inputFileName)
		cfgfile = path.join(jobpath, cfgFileName)
		detlist = path.join(jobpath, 'DetectorList.xml')
		detprop = path.join(jobpath, 'DetectorProperties.xml')
		outfile = path.join(jobpath, outputFileName)

		# flags to be replaced in the configuration file
		flags = {"@INPUTFILE@" : infile,
						 "@NPARTICLES@" : nlines,
						 "@OUTPUTFILE@" : outfile,
						 "@DETECTORLIST@" : detlist,
						 "@DETECTORPROPERTIES@" : detprop
						 }

		# read json file and replace the key flags
		fin = open(cfgfile, "rt")
		data = json.load(fin)
		
		# copy original dictionary
		copydata = data

		# replace values in copy
		# the following lines may need to be changed if the strcuture of the original JSON file changes.
		copydata['Input']['InputFileName'] = flags['@INPUTFILE@']		
		copydata['Input']['InputNParticles'] = flags['@NPARTICLES@']
		copydata['Output']['OutputFile'] = flags['@OUTPUTFILE@']
		copydata['DetectorList'] = flags['@DETECTORLIST@']
		copydata['DetectorProperties'] = flags['@DETECTORPROPERTIES@']

		# check that copy and original have the same keys
		if not data.keys() == copydata.keys():
			raise Exception("[ERROR] Copy of JSON content failed.")
			return False
		fin.close()
		# if we get here the copy was successful. write to file replacing original
		with open(cfgfile, "w") as outcfgfile:
			json.dump(copydata, outcfgfile)

		# dump job information into log file
		jobLogName = path.join(jobpath, 'job_info_%s.log' %jobid)
		with open(jobLogName, "w") as f:
			f.write("executable %s \n" %exeName)
			f.write("config_file %s \n" %cfgFileName)
			f.write("finished 0")

		f.close()

	print("[INFO] Done!")
	return True

def main():

	# parsers go here
	# write lines below into a main function
	# write parser for input, base and job path

	"""
	The parser should call:

	- input file type (extension, .shw / .bz2) (if .bz2, decompress)
	- input file directory
	- base directory
	- job directory 

	optional inputs

	"""

	parser = argparse.ArgumentParser(description='This script creates job directories to run multiple simulations in parallel.', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	#parser.add_argument("-e", dest="inext",
		#help="input file extension", default=None, required=True)
	parser.add_argument("-i", dest="infile",
		help="full path to input file and filename with extension. Example: /home/user/file.shw", default=None, required=False)
	parser.add_argument("-n", dest="nlines",
		help="number of lines to split input file", default=100000)
	parser.add_argument("-p", dest="inpart",
		help="number of particles to be simulated in case no input file is given", default=0, required=False)

	parser.add_argument("-b", dest="basedir",
		help="base directory (containing Executable and cfg files)", default=None, required=True)
	parser.add_argument("-j", dest="jobdir",
		help="jobs directory", default="jobs")

	# needed argument with output directory which collects all outputs
	# parser.add_argument("-o", dest="outdir",
	# help="output directory", default="output")

	options = parser.parse_args()

	# print options given to the parser
	# input_dir = abspath(options.indir)
	base_dir  = abspath(options.basedir)
	job_dir   = abspath(options.jobdir)
	input_file = options.infile
	input_part = int(options.inpart)

	nlines = int(options.nlines)
	#filext    = options.inext
	if not input_file and not input_part:
		print("[ERROR] Input file or Input number of particles must be given!")
		return False

	print("[INFO] Running createjobs script with the following options:\n")
	if input_file:
		print("- Path to input file: %s " %input_file)
	if input_part:
		print("- Number of particles to be simulated: %i " %input_part)

	print("- Path to base directory: %s" %base_dir)
	print("- Path to job directory: %s" %job_dir)
	print("\n")

	# implement check if split files already exist.
	# if not: split. if exist: jump to CreateJobDirectories
	if input_file:
		print("[INFO] Reading input file")
		print(input_file)
		filext = path.splitext(input_file)[1]
	
		if filext == "bz2":
			print("[INFO] Decompressing file %s" %input_file)
			os.system("bzip2 -d -k %s" %input_file)
			# now take the decompressed file as input
			input_file = path.splitext(input_file)[0]
	
		print("\n")	
		splits_dir = SplitInputFile(input_file, N=nlines)

		try:
			jobDirs = CreateJobs(base_dir, splits_dir, job_dir)
			if PrepareConfigFile(jobDirs, 0):
				print("[INFO] Configured jobs at %s was done." %job_dir)
		except Exception as e:
			
			print("%s" %e)
			shutil.rmtree(splits_dir)
			print("[INFO] Deleting temporary directory %s" %splits_dir)

	if input_part:
		print("[INFO] Creating job directories for simulating %i particles" %input_part)
		print("\n")
		
		try:
			jobDirs = CreateJobsNPart(base_dir, job_dir, input_part, n_max=nlines)
			if PrepareConfigFile(jobDirs, nlines):
				print("[INFO] Configured jobs at %s was done." %job_dir)
		except Exception as e:
			print("%s" %e)


if __name__ == "__main__":

	main()

