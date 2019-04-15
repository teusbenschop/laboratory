#!/usr/bin/env python

class User:
  name = ""

  def __init__(self, name):
    self.name = name

  def sayHello(self):
    print ("Hello, my name is " + self.name)

# Create objects
james = User("James")
david = User("David")
eric = User("Eric")

# Call methods owned by objects
james.sayHello()
david.sayHello()
eric.sayHello ()

