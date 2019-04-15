#!/usr/bin/env python

# https://pythonspot.com/python-lambda/

f = lambda x : 2 * x + 3
print (f(3))

list = [1,2,3,4,5]
squaredList = map(lambda x: x*x, list)
print(squaredList)

list = [1,2,3,4,5,6,7,8,9,10]
newList = filter(lambda x: x % 2 == 0, list)
print(newList)

