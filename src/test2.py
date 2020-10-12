#!/usr/bin/python3
# -*- coding: utf-8 -*-
"""
Created on Mon Sep 28 21:14:44 2020

@author: par116
@author: mfb31
"""

import subprocess
from subprocess import PIPE
import sys
import os
import time
import re
import math
import matplotlib.pyplot as plt

def test():
	threadStep = [0, 1, 2, 4, 8, 16, 32]    # number of threads per size range 
	size = 501                              # cube size
	iterationsMin = 500                     
	iterationsMax = 500
	threadCount = 0
	listThreads = []
	listSizeTime = []

	open('output.txt', 'w').close()

	cmd = './poisson_test'
	output_file = open("output.txt", "a")


	for threadNum in threadStep:
		for array_size in range (101, size+1, 100):
			for iteration in range (iterationsMin, iterationsMax+1, 100):
			
				start = time.time()
				result = subprocess.run(['time', cmd, format(array_size), format(iteration), format(threadNum)], stdout=PIPE, stderr=PIPE, universal_newlines=True)
				end = time.time()
				total_time = end - start
				output_file.write('Thread count: ' + format(threadNum) + ' Size: ' + format(array_size) + '  Iterations: ' + format(iteration) + '\n')
				#output_file.write(result.stderr + '\n')
				output_file.write('Time (s): ' + format(total_time) + '\n')
				print(result)

				listSizeTime.append(array_size)
				listSizeTime.append(total_time)

		listThreads.append(listSizeTime)

	output_file.close()
		#print(result.stdout)
		#print(result.stderr)

	# For test only, check output     
	print(listThreads)
"""
	# Slice list to get times versus array size, for given thread count
	# Then Plot data
	for threadNum in range (0, len(listThreads) ):
		aThread = list(map(lambda x: x[threadNum], listThreads))    # slice to get set of results for each thread count
		aSize = list(map(lambda x: x[0], aThread))                  # slice to get size values (x axis)
		aTime = list(map(lambda x: x[1], aThread))                  # slice to get time values (y axis)
		plt.plot(aSize, aTime, 'r.--', markersize=1)                # plot

	plt.title('Completion time per cube size, per thread count')
	plt.legend(["0", "1", "2" ,"4", "8", "16", "32"], loc='upper left')
	plt.xlabel('Cube size')
	plt.ylabel('Time (S)')
	plt.grid(b=True, which='major', color='#888888', linestyle='-')
	plt.minorticks_on()
	plt.grid(b=True, which='minor', color='#999999', linestyle='dotted', alpha=0.5)
	plt.show()

	#result.stdout >> 'output.txt'

	#os.system('cmd /k "ls -l >> output.txt"')
""" 
if __name__ == "__main__":
    test()
    
