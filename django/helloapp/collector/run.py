import psycopg2
import time
import datetime
from urllib.request import urlopen
import json

try:
  connection = psycopg2.connect (user = "postgres_user",
                                 password = "postgres_pass",
                                 host = "127.0.0.1",
                                 port = "5432",
                                 database = "my_postgres_db")

  print (connection.get_dsn_parameters())

  cursor = connection.cursor()
  cursor.execute("SELECT version();")
  record = cursor.fetchone()
  print (record)

  while (True):
    
    try:

      response = urlopen ("http://weerlive.nl/api/json-data-10min.php?key=demo&locatie=Amsterdam")
      # print (response)
      data = response.read().decode("utf-8")
      json_object = json.loads(data)
      liveweather = json_object ['liveweer'];
      # print (liveweather)
      location = liveweather [0]['plaats'];
      temperature = liveweather [0]['temp'];
      print (location, temperature, "°C")
      
      insert_query = """ INSERT INTO weather (location, temperature, stamp) VALUES (%s, %s, NOW())"""
      record_to_insert = (location, temperature)
      cursor.execute (insert_query, record_to_insert)
      connection.commit()
      count = cursor.rowcount
      print (datetime.datetime.now(), count, "record inserted successfully")

    except (Exception) as error:
      print (error)
    
    time.sleep (600)

except (Exception, psycopg2.Error) as error :
  print (error)
finally:
  if (connection):
    cursor.close()
    connection.close()
    print("PostgreSQL connection is closed")

