#!/usr/bin/env python

# https://pythonspot.com/inheritance/

class User:
  name = ""

  def __init__(self, name):
    self.name = name

  def printName(self):
    print ("Name is " + self.name)

class Programmer(User):

  def __init__(self, name):
    self.name = name
  def doPython(self):
    print ("Programming Python")

brian = User("brian")
brian.printName()

diana = Programmer("Diana")
diana.printName()
diana.doPython()

