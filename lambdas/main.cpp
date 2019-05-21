#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <limits>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <string>


using namespace std;


int main (int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  // [] is capture list
  // () is function argument provider
  // {} is lambda implementation
  auto f1 = [] ()
  {
    cout << "f1" << endl;
  };
  f1 ();

  // Calling the lambda function by appending ()
  [] () { cout << "f2" << endl; } ();

  // Lambda function returning an integer.
  auto f3 = [] ()->int { return 1; };
  cout << f3 << endl;

  // Getting the function reference using std::function<>
  function<int(int)> f4 = [] (int value)->int {
    return value + 1;
  };
  int i4 = f4 (100);
  cout << i4 << endl;

  return EXIT_SUCCESS;
}
