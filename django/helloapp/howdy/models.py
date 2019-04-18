from django.db import models

import psycopg2

def get_current_temperature ():

  connection = psycopg2.connect (user = "postgres_user",
                                 password = "postgres_pass",
                                 host = "127.0.0.1",
                                 port = "5432",
                                 database = "my_postgres_db")
  cursor = connection.cursor()
  query = "SELECT temperature FROM weather ORDER BY id DESC LIMIT 1;"
  cursor.execute (query)
  record = cursor.fetchone()
  temperature = record[0]

  if (connection):
    cursor.close()
    connection.close()
  return temperature


