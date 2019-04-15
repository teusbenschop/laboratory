#!/usr/bin/env python

# https://pythonspot.com/inner-classes/

class Human:

  def __init__(self):
    self.name = 'Guido'
    self.head = self.Head()

  class Head:
    def talk(self):
      return 'talking...'

guido = Human()
print (guido.name)
print (guido.head.talk())
