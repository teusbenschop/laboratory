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


class Dog:
  
  tricks = []             # mistaken use of a class variable
  
  def __init__(self, name):
    self.name = name
    
  def add_trick(self, trick):
    self.tricks.append(trick)

d = Dog('Fido')
e = Dog('Buddy')
d.add_trick('roll over')
e.add_trick('play dead')
print (d.tricks)                # unexpectedly shared by all dogs


class Employee:
  pass

john = Employee()  # Create an empty employee record

# Fill the fields of the record
john.name = 'John Doe'
john.dept = 'computer lab'
john.salary = 1000
print (john.name)


try:
  s = 'abc'
  it = iter(s)
  print (it)
  print (next(it))
  print (next(it))
  print (next(it))
  print (next(it))
  print (next(it))
except StopIteration as err:
  print ("StopIteration", format(err))

