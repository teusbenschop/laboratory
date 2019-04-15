#!/usr/bin/env python

class Human:

  def sayHello(self, name = None):

    if name is not None:
      print ('Hello ' + name)
    else:
      print ('Hello')

obj = Human()
obj.sayHello()
obj.sayHello('Guido')
