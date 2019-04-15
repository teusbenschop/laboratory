#!/usr/bin/env python

# https://pythonspot.com/threading/

# This source file should not be named "threading.py"
# Because that leads to errors,
# sincee it importa "threading".

import threading

class MyThread (threading.Thread):
  
  def __init__(self,x):
    self.__x = x
    threading.Thread.__init__(self)
    
  def run (self):
    print (str(self.__x))

for x in [1,2,3,4,5,6,7]:
  MyThread(x).start()

