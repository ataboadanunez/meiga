#!/usr/bin/env python3

# description
# author
# date

import argparse
import multiprocessing as mp
#from threading import Thread, current_thread
import subprocess
import time

import tempfile
import shutil
import sys
import os
import glob
import time

from createjobs import abspath

from IPython import embed

def check_for_done(joblist):
	for j, p in enumerate(joblist):
		if p.poll() is not None:
			return True, j

	return False, False


def prepare_queue(jobdir):

	queue = []
	# checks if jobs.log exists
	# open jobs.log and returns list with job directories

	jobslog = os.path.join(jobdir, 'jobs.log')
	if not os.path.exists(jobslog):
		raise	Exception("[ERROR] jobs.log file does not exist in directory %s" %jobdir)

	joblist = []
	with open(jobslog, "r") as f:
		for line in f:
			l = str.split(line)
			if l[0] == '#':
				continue
		
			joblist.append(l[0])

	f.close()

	# loop over job list and open job_info_##.log
	# save the process in the queue

	for jobid, jobpath in enumerate(joblist):

		logs = glob.glob(os.path.join(jobpath, 'job_info*.log'))
		if len(logs) == 0:
			raise Exception("[ERROR] job_info_##.log file was not found under %s" %jobpath)

		log = logs[0]

		# now get executable and config file name
		exe = ''
		cfg = ''
		with open(log, 'r') as f:
			for line in f:
				l = str.split(line)
				if l[0] == 'executable':
					exe = l[1]
				if l[0] == 'config_file':
					cfg = l[1]
			
		f.close()

		runcmd = '%s -c %s' %(exe, cfg)
		logpath = os.path.join(jobpath, 'out.log')
		#process = ["%s/%s" %(jobpath, exe), "-c", "%s" %cfg, "> %s 2>&1 &" %logpath ]
		process = ["%s/%s" %(jobpath, exe), "%s" %cfg, "%s" %logpath]
		queue.append(process)
	
	return queue


def run(proc):
	exe = proc[0] # G4MuDecSimulator*
	cfg = proc[1] # G4MuDecSimulator.json
	log = proc[2] # log.out

	print('Running executable %s' %exe)
	#cmd = "%s" + "-c" + "%s" + "> %s" %(exe, cfg, log)
	with open(log, "w+") as f:
		theprocess = subprocess.Popen([exe, "-c", cfg], shell=False, stdout=f, stderr=subprocess.STDOUT)
	#subprocess.check_call(["%s" %exe, "-c", "%s" %cfg, "> %s" %log])

	return theprocess


def processData(queue, table):
	
	proc = queue.get()
	#print(proc)
	run(proc)

	print("Worker", current_thread().ident, "finished")

def worker(queue):

	pids = []

	for proc in iter(queue.get, None):
		try:
			pid = run(proc)
			pids.append(pid)

		except Exception as e:
			print('[ERROR] %r failed: %s' %(proc[0], e), file=sys.stderr)

	print (pids)


def main():
	
	parser = argparse.ArgumentParser(description='This script runs multiple simulations in parallel.', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("-j", dest="jobdir",
		help="directory with jobs", default=None, required=True)
	parser.add_argument("-n", dest="nthreads",
		help="number of threads to use", default=None)

	options = parser.parse_args()

	job_dir = abspath(options.jobdir)

	if options.nthreads == None:
		n_threads = mp.cpu_count()-1
	else:
		n_threads = int(options.nthreads)

	print("[INFO] Running runjobs with the following options:\n")
	print("- Path to job directory: %s" %job_dir)
	print("- Number of threads: %i" %n_threads)

	print("\n")

	print("[INFO] Preparing queue...")
	try:
		joblist = prepare_queue(job_dir)
		
	except Exception as e:
		print("%s" %e)
		print("Exiting...")
		return

	processes = []

	queue_size = min(n_threads, len(joblist))
	print("[INFO] Total number of jobs: %i" %len(joblist))
	print("[INFO] Queue max size: %i" % queue_size)

	while len(joblist) > 0:
		#Monitor ending processes
		for p in processes:
			if p.poll() != None: processes.remove(p)

		#Start all "free" processes
		while len(processes) < queue_size and len(joblist) > 0:

			#embed()
			#while (true); do ps size ID; sleep 600; done > mem.log 
			# ps aux | grep meiga
			processes.append(run(joblist.pop(0)))

		#Take a well deserved break
		time.sleep(15)

	#All jobs have either run or begun execution, wait() the stragglers
	for p in processes:
		p.wait()


		# while queue is not empty, load it with processes

	#n_jobs = myQueue.qsize()
	#print("[INFO] Running %i jobs using %i threads" %(n_jobs, n_threads))
	
	


if __name__ == "__main__":

	main()


if False:

	processes = list()
	queue = list()
	queue = prepare_queue(jobdir, queue)
	# main, run processes in queue

	for process in queue:
		##p = subprocess.Popen(process)

		
		processes.append(os.system("./%s -c %s > %s 2>&1 &" %(process[0], process[1], process[2])))

		embed()
		if len(processes) == numprocesses:
			wait = True
			while wait:
				done, num = check_for_done(processes)

				if done:
					processes.pop(num)
					wait = False
				else:
					time.sleep(0.5)




