Learn Python
============

https://pythonspot.com
https://python.org
https://docs.python.org/3/tutorial


Create the web app
==================

Tutorial at https://scotch.io/tutorials/build-your-first-python-and-django-application

$ pip install django
$ django-admin startproject helloapp
$ python manage.py createsuperuser
$ cd django/helloapp
$ python3 manage.py runserver 192.168.2.15:8000


Install PostgreSQL
==================

https://www.digitalocean.com/community/tutorials/how-to-install-and-use-postgresql-on-ubuntu-18-04
https://www.digitalocean.com/community/tutorials/how-to-create-remove-manage-tables-in-postgresql-on-a-cloud-server

sudo su - postgres_user
psql my_postgres_db

CREATE TABLE weather (
  id serial PRIMARY KEY,
  location varchar (50) NOT NULL,
  temperature float NOT NULL,
  stamp timestamp NOT NULL
);

insert into weather (location, temperature, stamp) values ('Foo', 10, NOW());
select * from weather;


Collect data
============

Update the crontab on Ubuntu:
*/10 * * * * python3 django/helloapp/collector/run.py


