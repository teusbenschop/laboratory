#!/usr/bin/env python

# https://pythonspot.com/python-database-programming-sqlite-tutorial/

import sqlite3 as lite

con = None
con = lite.connect('sqlite.db')
cur = con.cursor()
cur.execute('SELECT SQLITE_VERSION()')
data = cur.fetchone()
print ("SQLite version: %s" % data)

with con:
  cur = con.cursor()
  cur.execute("CREATE TABLE Users(Id INT, Name TEXT)")
  cur.execute("INSERT INTO Users VALUES(1,'Michelle')")
  cur.execute("INSERT INTO Users VALUES(2,'Sonya')")
  cur.execute("INSERT INTO Users VALUES(3,'Greg')")

with con:
  cur = con.cursor()
  cur.execute("SELECT * FROM Users")
  rows = cur.fetchall()
  for row in rows:
    print (row)

if con:
  con.close()
