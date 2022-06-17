# description
# author
# date

import argparse
import random
import re
import string

from glob import glob
import numpy as np
import os
from os import path, mkdir, makedirs, getcwd, chmod, listdir, environ, popen, system
from sys import stdout, argv

import tempfile
import shutil

from IPython import embed


def abspath(s):
	if s[0] == '/':
		return s
	cwd = environ["PWD"]
	return path.join(cwd, s)

def SplitInputFile(ifile, N=100000):
	# check that input file exits
	if not path.exists(ifile):
		print("[ERROR] Input file not exists!")
		return

	print("[INFO] Splitting input file: %s" %ifile)
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


def CreateJobs(base_dir, splits_dir, job_dir):
	
	# copies executable and .json / .xml files from base_dir to each job_dir/jobid
	# replaces "inputfile" target in .json with corresponding split file

	# returns file "joblists.txt" with list of job_dir/jobid 


	# check if base_dir exists
	if not path.exists(base_dir):
		print("[WARNING] Base directory does not exist. Exiting ...")
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
		jobpath = path.join(job_dir, 'run'+jobid)
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
	jobLogName = path.join(job_dir, 'jobs.log')
	with open(jobLogName, "w") as f:
		f.write("CREATED JOBS: \n")
		for jobi in jobDirs:
			f.write(jobi + "\n")

	f.close()

	shutil.rmtree(splits_dir)
	print("[INFO] Deleting temporary directory %s" %splits_dir)
	
	return jobDirs


def PrepareConfigFile(job_dir):

	print("[INFO] Preparing Configuration files")
	def is_exe(filename):
		return os.access(filename, os.X_OK)

	for jobpath in job_dir:

		# list elements of that directory
		# find executable, config file (.json) and input file  

		cfgFileName = ''
		exeName = ''
		inputFileName = ''

		for item in listdir(jobpath):
			filename = path.join(jobpath, item)
			split_filename = path.splitext(filename)
			

			name = split_filename[0].split('/')[-1].split('_')[0]
			ext = split_filename[1]

			if ext == '.json':
				cfgFileName = filename
			
			if ext == '' and is_exe(filename):
				exeName = name

			if name == 'input':
				jobid = split_filename[0].split('/')[-1].split('_')[1]
				inputFileName = name + '_'+ jobid

		if not path.exists(path.join(jobpath,exeName)):
			raise Exception("[ERROR] Executable not found at %s" %jobpath)

		cfgfile = path.join(jobpath, cfgFileName)
		system("sed -i 's/@INPUTFILE@/%s/g' %s" %(inputFileName, cfgFileName))

	print("[INFO] Done!")
	return True


# write lines below into a main function
# write parser for input, base and job path

input_dir = '/home/alvaro/data/sims/casposo/casp_2505601/'
job_dir = abspath('test_job')
base_dir = abspath('test_base')

# implement check if split files already exist.
# if not: split. if exist: jump to CreateJobDirectories
ifiles = glob(path.join(input_dir, '*.shw'))

if len(ifiles) == 0:
	raise Exception("[ERROR] No input files!")

for ifile in ifiles:
	splits_dir = SplitInputFile(ifile)
	jobDirs = CreateJobs(base_dir, splits_dir, job_dir)
	if PrepareConfigFile(jobDirs):
		print("[INFO] Configured jobs at %s was done." %job_dir)

