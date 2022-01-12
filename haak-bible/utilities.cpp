#include "libraries.h"
#include "utilities.h"


vector <string> scandir (string folder)
{
  vector <string> files;
  DIR * dir = opendir (folder.c_str());
  if (dir) {
    struct dirent * direntry;
    while ((direntry = readdir (dir)) != NULL) {
      string name = direntry->d_name;
      // Exclude short-hand directory names.
      if (name == ".") continue;
      if (name == "..") continue;
      // Exclude developer temporal files.
      if (name == ".deps") continue;
      if (name == ".dirstamp") continue;
      // Exclude macOS files.
      if (name == ".DS_Store") continue;
      // Store the name.
      files.push_back (name);
    }
    closedir (dir);
  }
  sort (files.begin(), files.end());
  // Remove . and ..
  files = array_diff (files, {".", ".."});
  return files;
}


vector <string> array_diff (vector <string> from, vector <string> against)
{
  vector <string> result;
  set <string> against2 (against.begin (), against.end ());
  for (unsigned int i = 0; i < from.size (); i++) {
    if (against2.find (from[i]) == against2.end ()) {
      result.push_back ((from[i]));
    }
  }
  return result;
}


string get_home_folder ()
{
  string homefolder;
  const char * homeptr = getenv ("HOME");
  if (homeptr) homefolder = homeptr;
  return homefolder;
}


string create_path (string part1, string part2, string part3, string part4, string part5, string part6)
{
  string path (part1);
  if (part2.length()) path += "/" + part2;
  if (part3.length()) path += "/" + part3;
  if (part4.length()) path += "/" + part4;
  if (part5.length()) path += "/" + part5;
  if (part6.length()) path += "/" + part6;
  return path;
}
