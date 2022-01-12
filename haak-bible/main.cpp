#include "libraries.h"
#include "utilities.h"

using namespace std;

string usfm_path;

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  usfm_path = create_path (get_home_folder (), "work/bibledit/server/exports/haak-bible/usfm/full");
  vector <string> files = scandir (usfm_path);
  for (auto file : files) {
    if (file.find (".zip") != string::npos) continue;
    cout << file << endl;
  }
  return 0;
}
  