#!/usr/bin/env python

import os.path

filename = "read.py"
content = ""

if not os.path.isfile(filename):
  print ('File does not exist')
else:
  with open(filename) as f:
    content = f.read().splitlines()

for line in content:
  print(line)
