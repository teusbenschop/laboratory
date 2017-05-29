#include <config.h>
#include <libgen.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;

int main (int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  while (true) {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time (localtime(&in_time_t), "%Y-%m-%d %X");
    string s = ss.str();
    cout << s << endl;
    ofstream file;
    file.open ("/tmp/cloud.txt", ios::app);
    file << s << endl;
    file.close ();
    this_thread::sleep_for (chrono::seconds (1));
  }
  return 0;
}
