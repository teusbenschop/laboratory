/*
Copyright (©) 2003-2023 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#pragma once

#include <string>
#include <map>
#include <sqlite3.h>

sqlite3 * database_sqlite_connect_file (std::string filename);
std::string database_sqlite_file (std::string database);
std::string database_sqlite_suffix ();
sqlite3 * database_sqlite_connect (std::string database);
std::string database_sqlite_no_sql_injection (std::string sql);
void database_sqlite_exec (sqlite3 * db, std::string sql);
std::map <std::string, std::vector <std::string> > database_sqlite_query (sqlite3 * db, std::string sql);
void database_sqlite_disconnect (sqlite3 * database);
bool database_sqlite_healthy (std::string database);
void database_sqlite_error (sqlite3 * database, const std::string & prefix, char * error);

// Creates a database SQL query.
class SqliteSQL
{
public:
  void clear ();
  void add (const char * fragment);
  void add (int value);
  void add (std::string value);
  std::string sql {};
private:
};

// Stores values collected during a reading session of sqlite3.
class SqliteReader
{
public:
  SqliteReader (int dummy);
  ~SqliteReader ();
  std::map <std::string, std::vector <std::string> > result {};
  static int callback (void *userdata, int argc, char **argv, char **column_names);
private:
};

class SqliteDatabase
{
public:
  SqliteDatabase (std::string filename);
  ~SqliteDatabase ();
  SqliteDatabase(const SqliteDatabase&) = delete;
  SqliteDatabase operator=(const SqliteDatabase&) = delete;
  void clear ();
  void add (const char * fragment);
  void add (int value);
  void add (std::string value);
  std::string sql {};
  void execute ();
  std::map <std::string, std::vector <std::string> > query ();
private:
  sqlite3 * db {nullptr};
};
