#!/usr/bin/env python

# Don't name this "random.py"
# because it imports from there,
# so it won't work.
# Give it a different name, this file.

from random import *

print (randint (0, 1))

items = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
shuffle(items)
print (items)

items = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

x = sample(items,  1)   # Pick a random item from the list
print (x[0])

y = sample(items, 4)    # Pick 4 random items from the list
print (y)

