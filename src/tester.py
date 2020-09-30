#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Sep 28 21:14:44 2020

@author: par116
"""

import subprocess
import sys
import os

cmd = './poisson_test'

#result = subprocess.run(['g++', '-03', 'std=c++11', '-Wall', '-g3', '-o', '$@', '$^', cmd, '100', '100'], capture_output=True, text=True)
#result = subprocess.run(['time', cmd, '100', '100'], capture_output=True, text=True)
#print(result.stdout)
#print(result.stderr)

import sys
print("argc =", len(sys.argv))
print("argv =", sys.argv)
