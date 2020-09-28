#
# poisson_grapher
#
# Mark & Manu
# ENCE464-20S2 group 15
# 2020_09_28
#
# This script runs repeat calls of poisson_test program with differing input variables,
#  (threads and size)
#  captures the text outputs and graphs it.
#  It also makes use of cachegrind fotr cache mises / hits / miss rate 
#


import os
import re
import math
import subprocess
import sys
from statistics import variance
from statistics import mean
from statistics import stdev
import matplotlib.pyplot as plt


def poisson_grapher():
    print("")
    
    # set up variables and structures  
    numErrors = 0
    listRunTimes = []
    listL1CacheMisses = []
    #listL1CacheHits = []
    listMissRateL1 = []
    maxSize = 201   #501
    maxThreads = 4  #64
    iterations = 500
    

    # iterate through number of threads
    threadStep = 0
    for threadNum in range ( 2, maxThreads + 1, (2 ** threadStep) ):

        # iterate through sizes
        for inputSize in range (101, maxSize + 1, 100)

            result = subprocess.run(
                [sys.executable, "-c", "print('ocean')"], capture_output=True, text=True)

            for myline in result.stdout:              # For each line, read to a string,
                    #print(myline)                  # and print the string.
                    #myList = list(myline)

                    # Find times
                    # rather clunky, tidy up later
                    anIndex = myline.find('user')
                    if anIndex != -1:                           # first check we have a time
                        timing = myline.partition('user')[2]

                    missesIndex = myline.find('D1 misses')
                    if missesIndex != -1
                        L1misses = myline.partition('user')[2]

                    missRateIndex = myline.find('D1 miss rate')
                    if missRateIndex != -1:
                        L1missRate = myline.partition('user')[2]

            listRunTimes.append(timing)
            listL1CacheMisses.append(L1misses)
            #listL1CacheHits.append()
            listL1MissRate.append(L1missRate)

        threadStep++

    print(listRunTimes)
    print(listL1CacheMisses)
    print(listL1CacheHits)
    print(listMissRateL1)