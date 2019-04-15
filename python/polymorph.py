#!/usr/bin/env python

# https://pythonspot.com/polymorphism

class Bear(object):
  def sound(self):
    print ("Groarrr")

class Dog(object):
  def sound(self):
    print ("Woof woof")

def makeSound(animalType):
  animalType.sound()

bearObj = Bear()
dogObj = Dog()

makeSound(bearObj)
makeSound(dogObj)


class Document:
  def __init__(self, name):
    self.name = name

  def show(self):
    raise NotImplementedError("Subclass must implement abstract method")

class Pdf(Document):
  def show(self):
    return 'Show pdf contents'

class Word(Document):
  def show(self):
    return 'Show word contents'

documents = [Pdf('Document1'),
             Pdf('Document2'),
             Word('Document3')]

for document in documents:
  print (document.name + ': ' + document.show())
