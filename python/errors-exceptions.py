#!/usr/bin/env python

# https://docs.python.org/3/tutorial/errors.html

# while True print('Hello world')

while True:
  try:
    x = int(input("Please enter a number: "))
    break
  except ValueError as err:
    print(format(err) + ". Try again...")

# raise NameError('HiThere')

