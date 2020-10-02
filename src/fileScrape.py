"""

@author: mfb31
"""

import sys
import os
import time
import re
import math
import matplotlib.pyplot as plt

def fileScrape():
    with open ("output.txt", "rt") as myfile:

        listThreads = []
        listTimes = []
        listSizes = []

        for myline in myfile:              # For each line, read to a string,
            # Find thread value
            # rather clunky, tidy up later
            anIndex = myline.find('Thread count: ')
            if anIndex != -1:
                newInt = anIndex + len('Thread count: ')
                intText = myline[newInt:]
                anInt, sep, tail = intText.partition(' ')
                threadNum = int(anInt)
                listThreads.append(threadNum)
                #print

            nextIndex = myline.find('Size: ')
            if nextIndex != -1:
                necInt = nextIndex + len('Size: ')
                moreText = myline[necInt:]
                valInt, sep, tail = moreText.partition(' ')
                sizeInt = int(valInt)

                listSizes.append(sizeInt)
                #print

            lastIndex = myline.find('Time (s): ')
            if lastIndex != -1:
                newFloat = lastIndex + len('Time (s): ')
                floatText = myline[newFloat:]
                valFloat, sep, tail = floatText.partition(' ')
                timeFloat = float(valFloat)

                listTimes.append(timeFloat)
                #print

            #print(listThreads)
            #print(listTimes)
            #print(listSizes)

    # For test only, check output     
    print(listThreads)
    print(listSizes)
    print(listTimes)

    # Slice list to get times versus array size, for given thread count
    # Then Plot data
    """
    for threadNum in range (0, len(listThreads) ):
        aThread = list(map(lambda x: x[threadNum], listThreads))    # slice to get set of results for each thread count
        aSize = list(map(lambda x: x[0], aThread))                  # slice to get size values (x axis)
        aTime = list(map(lambda x: x[1], aThread))                  # slice to get time values (y axis)
        plt.plot(aSize, aTime, 'r.--', markersize=1)                # plot
    """
    for scale in range (0, 6):
        plt.plot(listSizes[scale], listTimes[scale], 'r.--', markersize=1)                # plot

        plt.title('Completion time per cube size, per thread count')
        plt.legend(["0", "1", "2" ,"4", "8", "16", "32"], loc='upper left')
        plt.xlabel('Cube size')
        plt.ylabel('Time (S)')
        plt.grid(b=True, which='major', color='#888888', linestyle='-')
        plt.minorticks_on()
        plt.grid(b=True, which='minor', color='#999999', linestyle='dotted', alpha=0.5)
        plt.show()

if __name__ == "__main__":
    fileScrape()