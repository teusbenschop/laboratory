import psycopg2

try:
  connection = psycopg2.connect (user = "postgres_user",
                                 password = "postgres_pass",
                                 host = "127.0.0.1",
                                 port = "5432",
                                 database = "my_postgres_db")

  print ( connection.get_dsn_parameters())

  cursor = connection.cursor()
  cursor.execute("SELECT version();")
  record = cursor.fetchone()
  print (record)

except (Exception, psycopg2.Error) as error :
  print ("Error while connecting to PostgreSQL", error)
finally:
  if (connection):
    cursor.close()
    connection.close()
    print("PostgreSQL connection is closed")

