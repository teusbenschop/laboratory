#!/usr/bin/env python

# https://pythonspot.com/ftp-client-in-python/

import ftplib

ftp = ftplib.FTP("ftp.nluug.nl")
ftp.login("anonymous", "ftplib-example-1")

data = []

ftp.dir (data.append)

ftp.quit()

for line in data:
  print ("-", line)

