#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Sep 28 21:14:44 2020

@author: par116
"""

import subprocess
from subprocess import PIPE
import sys
import os
import time


#def main():
    
size = 350
iterations = 300
threads = 10

open('output.txt', 'w').close()

cmd = './poisson_test'
output_file = open("output.txt", "a")

for thread in range (2, threads, 2):
    for array_size in range (100, size, 50):
        for iteration in range (100, iterations, 100):
        
            start = time.time()
            result = subprocess.run(['time', cmd, format(array_size), format(iteration), format(thread)], stdout=PIPE, stderr=PIPE, universal_newlines=True)
            end = time.time()
            total_time = end - start
            output_file.write('Thread count: ' + format(thread) + ' Size: ' + format(array_size) + '  Iterations: ' + format(iteration) + '\n')
            #output_file.write(result.stderr + '\n')
            output_file.write('Time (s): ' + format(total_time) + '\n')
            print(result)
     
    #result = subprocess.run(['g++', '-03', 'std=c++11', '-Wall', '-g3', '-o', '$@', '$^', cmd, '100', '100'], capture_output=True, text=True)
    #result = subprocess.run(['time', cmd, '100', '100'])
    #result = subprocess.run(['valgrind', '--tool=cachegrind', cmd, '100', '100', '5'], stdout=PIPE, stderr=PIPE, universal_newlines=True)
    #result = subprocess.run(['time', cmd, '100', '100', '5'], stdout=PIPE, stderr=PIPE, universal_newlines=True)
    
    
output_file.close()
    #print(result.stdout)
    #print(result.stderr)
    
#result.stdout >> 'output.txt'


#os.system('cmd /k "ls -l >> output.txt"')
    
#if __name__ == "__main__":
#    main()
    