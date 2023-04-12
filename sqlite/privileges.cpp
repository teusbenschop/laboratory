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


#include <unistd.h>
#include "privileges.h"
#include "sqlite.h"
#include <vector>
using namespace std;


// The name of the database.
const char * Database_Privileges::database ()
{
  return "privileges";
}


void Database_Privileges::create ()
{
  SqliteDatabase sql (database ());
  
  sql.add ("CREATE TABLE IF NOT EXISTS bibles ("
           " username text,"
           " bible text,"
           " book integer,"
           " write boolean"
           ");");
  sql.execute ();
}


void Database_Privileges::upgrade ()
{
}


void Database_Privileges::optimize ()
{
  // Recreate damaged database.
  if (!healthy ()) {
    unlink (database_sqlite_file (database ()).c_str());
    create ();
  }
  // Vacuum it.
  SqliteDatabase sql (database ());
  // On Android, this pragma prevents the following error: VACUUM; Unable to open database file.
  sql.add ("PRAGMA temp_store = MEMORY;");
  sql.execute ();
  sql.clear ();
  sql.add ("VACUUM;");
  sql.execute ();
}


bool Database_Privileges::healthy ()
{
  return database_sqlite_healthy (database ());
}


void Database_Privileges::save (string username)
{
  SqliteDatabase sql (database ());
  
  vector <string> lines;

  sql.add ("SELECT book, write FROM table WHERE username =");
  sql.add (username);
  sql.add (";");
  map <string, vector <string> > result = sql.query ();
  vector <string> book =  result ["book"];
  vector <string> write = result ["write"];
  for (size_t i = 0; i < book.size (); i++) {
    lines.push_back (book [i]);
    lines.push_back (write[i]);
  }
}

