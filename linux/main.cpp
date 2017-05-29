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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>


using namespace std;

int main (int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  
  
  pid_t pid;
  // Fork off the parent process.
  pid = fork();
  // An error occurred.
  if (pid < 0) exit (EXIT_FAILURE);
  // Success: Let the parent terminate.
  if (pid > 0) exit (EXIT_SUCCESS);
  // On success: The child process becomes session leader.
  if (setsid() < 0) exit (EXIT_FAILURE);
  // Catch, ignore and handle signals.
  // TODO: Implement a working signal handler.
  signal (SIGCHLD, SIG_IGN);
  signal (SIGHUP, SIG_IGN);
  // Fork off for the second time.
  pid = fork ();
  // An error occurred
  if (pid < 0) exit (EXIT_FAILURE);
  // Success: Let the parent terminate.
  if (pid > 0) exit (EXIT_SUCCESS);
  // Write PID file.
  {
    ofstream file;
    file.open ("/var/run/bibledit-cloud.pid");
    pid = getpid ();
    file << pid << endl;
    file.close ();
  }
  // Set new file permissions.
  umask(0);
  // Change the working directory to the root directory or another appropriate directory.
  // chdir ("/");
  // Close all open file descriptors.
  for (int x = sysconf (_SC_OPEN_MAX); x >= 0; x--) close (x);
  // Open the log file.
  openlog ("bibledit-cloud", LOG_PID, LOG_DAEMON);
  
  syslog (LOG_NOTICE, "Bibledit Cloud started.");
  
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
  
  syslog (LOG_NOTICE, "Bibledit Cloud terminated.");
  closelog();
  
  return EXIT_SUCCESS;
}
