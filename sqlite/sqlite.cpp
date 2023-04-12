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


#include <mutex>
#include <vector>
#include <iostream>
#include "sqlite.h"

using namespace std;


// Despite compiling the SQLite library in thread-safe mode,
// there's still 'database locked' and "SQL error' errors.
// Sample errors:
// INSERT INTO cache VALUES ( 136 , 0 , '' ); - database is locked - database is locked
// INSERT INTO cache VALUES ( 25 , 21 , '' ); - unrecognized token: "'" - SQL logic error or missing database
// Therefore here's an extra mutex for our own logic, and that should fix those errors.
mutex sqlite_execute_mutex;


sqlite3 * database_sqlite_connect_file (string filename)
{
  sqlite3 *db {nullptr};
  int rc = sqlite3_open (filename.c_str(), &db);
  if (rc) {
    const char * error = sqlite3_errmsg (db);
    database_sqlite_error (db, "Database " + filename, const_cast<char*>(error));
    return nullptr;
  }
  sqlite3_busy_timeout (db, 1000);
  return db;
}


string database_sqlite_file (string database)
{
  return database;
}


string database_sqlite_suffix ()
{
  return ".sqlite";
}


sqlite3 * database_sqlite_connect (string database)
{
  return database_sqlite_connect_file (database_sqlite_file (database));
}


static string filter_string_str_replace (string search, string replace, string subject)
{
    size_t offposition = subject.find (search);
    while (offposition != string::npos) {
        subject.replace (offposition, search.length (), replace);
        offposition = subject.find (search, offposition + replace.length ());
    }
    return subject;
}


string database_sqlite_no_sql_injection (string sql)
{
  return filter_string_str_replace ("'", "''", sql);
}


void database_sqlite_exec (sqlite3 * db, string sql)
{
  char *error = nullptr;
  if (db) {
    sqlite_execute_mutex.lock ();
    int rc = sqlite3_exec (db, sql.c_str(), nullptr, nullptr, &error);
    sqlite_execute_mutex.unlock ();
    if (rc != SQLITE_OK) database_sqlite_error (db, sql, error);
  } else {
    database_sqlite_error (db, sql, error);
  }
  if (error) sqlite3_free (error);
}


map <string, vector <string> > database_sqlite_query (sqlite3 * db, string sql)
{
  char * error = nullptr;
  SqliteReader reader (0);
  if (db) {
    sqlite_execute_mutex.lock ();
    int rc = sqlite3_exec (db, sql.c_str(), reader.callback, &reader, &error);
    sqlite_execute_mutex.unlock ();
    if (rc != SQLITE_OK) database_sqlite_error (db, sql, error);
  } else {
    database_sqlite_error (db, sql, error);
  }
  if (error) sqlite3_free (error);
  return reader.result;
}


void database_sqlite_disconnect (sqlite3 * database)
{
  if (database) sqlite3_close (database);
}


// Does an integrity check on the database.
// Returns true if healthy, false otherwise.
bool database_sqlite_healthy (string database)
{
  string file = database_sqlite_file (database);
  bool ok = false;
  // Do an integrity check on the database.
  // An empty file appears healthy too, so deal with that.
  
  // Pseudo code:
  //if (filter_url_filesize (file) > 0) {
    sqlite3 * db = database_sqlite_connect (database);
    string query = "PRAGMA integrity_check;";
    map <string, vector <string> > result = database_sqlite_query (db, query);
    vector <string> health = result ["integrity_check"];
    if (health.size () == 1) {
      if (health [0] == "ok") ok = true;
    }
    database_sqlite_disconnect (db);
  //}
  return ok;
}


// Logs any error on the database connection,
// The error will be prefixed by $prefix.
void database_sqlite_error (sqlite3 * database, const string & prefix, char * error)
{
  string message = prefix;
  if (error) {
    message.append (" - ");
    message.append (error);
  }
  if (database) {
    int errcode = sqlite3_errcode (database);
    const char * errstr = sqlite3_errstr (errcode);
    string error_string;
    if (errstr) error_string.assign (errstr);
    int x_errcode = sqlite3_extended_errcode (database);
    const char * x_errstr = sqlite3_errstr (x_errcode);
    string extended_error_string;
    if (x_errstr) extended_error_string.assign (x_errstr);
    if (!error_string.empty ()) {
      message.append (" - ");
      message.append (error_string);
    }
    if (extended_error_string != error_string) {
      message.append (" - ");
      message.append (extended_error_string);
    }
    const char * filename = sqlite3_db_filename (database, "main");
    if (filename) {
      message.append (" - ");
      message.append (filename);
    }
  } else {
    if (!message.empty ()) message.append (" - ");
    message.append ("No database connection");
  }
    std::cerr << message << std::endl;
}


void SqliteSQL::clear ()
{
  sql.clear ();
}


void SqliteSQL::add (const char * fragment)
{
  sql.append (" ");
  sql.append (fragment);
  sql.append (" ");
}

void SqliteSQL::add (int value)
{
  sql.append (" ");
  sql.append (std::to_string (value));
  sql.append (" ");
}

void SqliteSQL::add (string value)
{
  sql.append (" '");
  value = database_sqlite_no_sql_injection (value);
  sql.append (value);
  sql.append ("' ");
}


SqliteReader::SqliteReader (int dummy)
{
  if (dummy) {};
}


SqliteReader::~SqliteReader ()
{
}


int SqliteReader::callback (void *userdata, int argc, char **argv, char **column_names)
{
  SqliteReader * sqlite_reader = static_cast<SqliteReader *> (userdata);
  for (int i = 0; i < argc; i++) {
    // Handle nullptr field.
    if (argv [i] == nullptr) sqlite_reader->result [column_names [i]].push_back ("");
    else sqlite_reader->result [column_names [i]].push_back (argv[i]);
  }
  return 0;
}


SqliteDatabase::SqliteDatabase (string filename)
{
  db = database_sqlite_connect (filename);
}


SqliteDatabase::~SqliteDatabase ()
{
  database_sqlite_disconnect (db);
}


void SqliteDatabase::clear ()
{
  sql.clear ();
}


void SqliteDatabase::add (const char * fragment)
{
  sql.append (" ");
  sql.append (fragment);
  sql.append (" ");
}


void SqliteDatabase::add (int value)
{
  sql.append (" ");
  sql.append (std::to_string (value));
  sql.append (" ");
}


void SqliteDatabase::add (string value)
{
  sql.append (" '");
  value = database_sqlite_no_sql_injection (value);
  sql.append (value);
  sql.append ("' ");
}


void SqliteDatabase::execute ()
{
  database_sqlite_exec (db, sql);
}


map <string, vector <string> > SqliteDatabase::query ()
{
  return database_sqlite_query (db, sql);
}
