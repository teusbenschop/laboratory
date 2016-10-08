#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <thread>

using namespace std;

void timer1 ()
{
  while (true) {
    cout << "thread one" << endl;
    this_thread::sleep_for (chrono::seconds (1));
  }
}

void timer2 ()
{
  while (true) {
    cout << "thread two" << endl;
    this_thread::sleep_for (chrono::seconds (1));
  }
}

int main ()
{
  new thread (timer1);
  this_thread::sleep_for (chrono::milliseconds (1));
  new thread (timer2);
  this_thread::sleep_for (chrono::milliseconds (1));
  int status;
  pid_t pid;
  if ((pid = vfork()) != 0) {
    if (pid < 0) {
      cout << "Parent - Child process failed" << endl;
    } else {
      cout << "Parent - Waiting for Child" << endl;
      if ((pid = wait(&status)) == -1) {
        cerr << "Parent - Wait failed" << endl;
      } else {
        cout << "Parent - Child final status: " << status << endl;
      }
    }
  } else {
    cout << "Parent - Starting Child" << endl;
    if ((pid = execlp ("sleep", "sleep", "5", (char *) NULL)) == -1) {
      cerr << "Parent - Execl failed" << endl;
      _exit(0);
    }
  }
  return 0;
}
