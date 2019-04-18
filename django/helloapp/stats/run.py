import psycopg2

connection = psycopg2.connect (user = "postgres_user",
                               password = "postgres_pass",
                               host = "127.0.0.1",
                               port = "5432",
                               database = "my_postgres_db")

cursor = connection.cursor()

cursor.execute("SELECT AVG (temperature) FROM weather;")
record = cursor.fetchone()
print ("Average temperature is", record[0], "degrees Celsius")

cursor.execute("SELECT MIN (temperature) FROM weather;")
record = cursor.fetchone()
print ("Minimum temperature is", record[0], "degrees Celsius")

cursor.execute("SELECT MAX (temperature) FROM weather;")
record = cursor.fetchone()
print ("Maximum temperature is", record[0], "degrees Celsius")

if (connection):
  cursor.close()
  connection.close()
