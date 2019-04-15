#!/usr/bin/env python

# https://pythonspot.com/recursion/

import sys

sys.setrecursionlimit(5000)

def factorial(n):
  if n == 1:
    return 1
  else:
    return n + factorial(n-1)

print (factorial(3000))
