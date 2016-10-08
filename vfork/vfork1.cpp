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

int main()
{
  int j, n, a, i, e, o, u;
  char str [50];
  a = e = i = o = u = 0;
  thread * t1 = new thread (timer1);
  this_thread::sleep_for (chrono::milliseconds (100));
  thread * t2 = new thread (timer2);
  this_thread::sleep_for (chrono::milliseconds (100));
  pid_t pid;
  if ((pid = vfork ()) < 0) {
    cerr << "vfork () error" << endl;
    exit (0);
  }
  if (pid == 0) {
    cout << "Counting number of vowels using vfork ()" << endl;
    cout << "The threads should continue running in the background" << endl;
    cout << "Enter the string: " << endl;
    fgets (str, sizeof (str), stdin);
    _exit (0);
  } else {
    n = strlen (str);
    for (j = 0; j < n; j++) {
      if (str [j] == 'a' || str [j] == 'A') a++;
      if (str [j] == 'e' || str [j] == 'E') e++;
      if (str [j] == 'i' || str [j] == 'I') i++;
      if (str [j] == 'o' || str [j] == 'O') o++;
      if (str [j] == 'u' || str [j] == 'U') u++;
    }
    cout << "Number of a's: " << a << endl;
    cout << "Number of e's: " << e << endl;
    cout << "Number of i's: " << i << endl;
    cout << "Number of o's: " << o << endl;
    cout << "Number of u's: " << u << endl;
    cout << "Total vowels : " << a + e + i + o + u << endl;
    exit (0);
  }
}
