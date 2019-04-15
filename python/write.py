#!/usr/bin/env python

filename = "write.txt"

myfile = open (filename, 'w')

myfile.write('Written with Python\n')

myfile.close()

# Append at the end of the file.
myfile = open(filename, 'a')

myfile.write('Written with Python\n')

myfile.close()
