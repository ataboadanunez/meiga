#!/usr/bin/env python3
# python script to merge multiple output files from RunJobs in a single file
# author: alvaro taboada
# date: 15.10.2023
import os
from os import path, environ, makedirs
import json
import gzip
import argparse
import glob

def abspath(s):
	if s[0] == '/':
		return s
	cwd = environ["PWD"]
	return path.join(cwd, s)

def update_event_ids(data, start_id, end_id):
	updated_data = {}
	for event_id, event_data in data.items():
			updated_event_id = f'Event_{start_id}'
			updated_data[updated_event_id] = event_data
			start_id += 1
	return updated_data, start_id

def MergeOutput(jobs_dir, merged_output_dir):
	if not path.exists(jobs_dir):
		print("[ERROR] Job directory %s does not exist! Exiting." %jobs_dir)
		exit()
	# check existence of jobs.log file
	jobslog_file = path.join(jobs_dir, 'jobs.log')
	hasLogFile = False
	if path.exists(jobslog_file):
			hasLogFile = True
			print("[INFO] Found %s file." %jobslog_file)
			with open(jobslog_file, 'r') as f:
					jobs_list = [item.split('\n')[0] for item in f.readlines() if not item.startswith('#')]

	if not hasLogFile:
		jobs_list = []
		hasJobDirs = False
		for file in os.listdir(jobs_dir):
				if (path.isdir(path.join(jobs_dir, file)) and file.startswith("run_")):
						hasJobDirs = True
						jobs_list.append(path.join(jobs_dir, file))
						print("[INFO] Found %s directory." %file)

	if not (hasLogFile or hasJobDirs):
			print("[ERROR] Jobs do not exist in %s directory. Neither `jobs.log` file nor `run_` were found. Exiting." %jobs_dir)
			exit()

	print("\n[INFO] Accessing job directories: ")
	# placeholder for meged data
	merged_data = {}
	merged_events = {}
	start_id = 0
	# loop over jobs
	for j, job in enumerate(sorted(jobs_list)):
		print(job)
		job_id_str = job.split('_')[-1]
		# read the log file inside each job directory
		log_file = path.join(job, 'job_info_%s.log' %job_id_str)
		if not path.exists(log_file):
			print("[WARNING] %s file does not exist. There should be a log file inside each job directory!" %log_file)
			continue
		# parse log file 
		log_entries = {}
		with open(log_file, 'r') as f:
			for line in f:
				parts = line.strip().split(' ', 1)
				# lines should only have two parts (identifier and path)
				if len(parts) == 2:
					identifier, pathname = parts
					log_entries[identifier] = pathname

			f.close()
		# parse config file
		config_file = log_entries['config_file']
		fin = open(config_file, "rt")
		cfgdata = json.load(fin)
		output_file = cfgdata['Output']['OutputFile']
		compress = cfgdata['Output']['CompressOutput']
		if compress:
			output_file += '.gz'
		fin.close()
		if not path.exists(output_file):
			print("[ERROR] Output file %s does not exist. Exiting." %output_file)
			exit()
		# open output file
		if compress:
			with gzip.open(output_file, 'rt', encoding='utf-8') as fo:
				data = json.load(fo)
		else:
			with open(output_file, 'rt') as fo:
				data = json.load(fo)
		det_list_data = data['DetectorList']
		event_data = data['Output']
		end_id = len(event_data)
		print("[INFO] Found output file %s. Number of events: %i" %(output_file, end_id))
		updated_data, start_id = update_event_ids(event_data, start_id, end_id)
		merged_events.update(updated_data)
		# keep original structure of JSON file
		merged_data['Output'] = merged_events
		merged_data['DetectorList'] = det_list_data

	# exporting data
	if not os.path.exists(merged_output_dir):
		print("[INFO] Creating directory %s" %merged_output_dir)
		makedirs(merged_output_dir)
		
	merged_output_file = path.join(merged_output_dir, 'output.json')
	with open(merged_output_file, 'w') as mf:
		json.dump(merged_data, mf, indent=4)

	print("[INFO] Output merged in %s." %merged_output_file)
	print("[INFO] Number of merged events: %i" %(len(merged_events)))

def main():
	parser = argparse.ArgumentParser(description='This script merges the output of multiple simulations ran with RunJobs in a single file.', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("-j", dest="jobdir",
		help="path to jobs directory", default=None, required=True)
	parser.add_argument("-o", dest="modir",
		help="name of merged output directory", default="./merged_output")
	
	options = parser.parse_args()
	merged_output_dir = abspath(options.modir)
	jobs_dir = abspath(options.jobdir)
	print("[INFO] Running mergeoutput script with the following options:\n")
	print("- Path to job directories: %s " %jobs_dir)
	print("- Path to output directory: %s\n" %merged_output_dir)

	MergeOutput(jobs_dir, merged_output_dir)
	print("[INFO] Exiting.")


if __name__ == '__main__':
	main()