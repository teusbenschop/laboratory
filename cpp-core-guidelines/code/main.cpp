#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <ranges>
#include <chrono>
#include <thread>
#include <cmath>
#include <mutex>
#include <numeric>
#include <functional>
using namespace std;
#include "main.h"

int main()
{
  print1 ();
  print2 ();
  print3 ();
  print4 ();
  print5 ();
  print6 ();
  print7 ();
  print8 ();
  return EXIT_SUCCESS;
}

void print1 ()
{
  try {
    vector<int> foo { 1, 2, 3, 4 };
    vector<int> bar {    2, 3, 4 };
    // This prints bar [4] which is beyond the vector.
    // It will print garbage instead or throw an exception.
    for (size_t i = 0; i < foo.size(); i++) {
      cout << foo[i] << " " << bar[i] << endl;
    }
  } catch (std::exception exception) {
    // Better to catch by const reference:
    // const std::exception & exception
    // Never catch a polymorphic type by value.
    // Cpp Core Guidelines E.15:
    // Throw by value, catch exceptions from a hierarchy by reference.
    cout << exception.what() << endl;
  }
}

void print2 ()
{
  try {
    const char * foo_ptr { nullptr };
    {
      string foo { "foo" };
      foo_ptr = foo.c_str();
    }
    cout << foo_ptr << endl;
    // The above was supposed to print "foo".
    // It will print garbage instead, or will throw an exception.
    // Reason: The object that foo_ptr points to has gone out of scope.
  } catch (const std::exception & exception) {
    cout << exception.what() << endl;
  }
}

void print3 ()
{
  float pi = 3.14159;
  float power = pi * pi;
  cout << power << endl;
  // The performance could be improved by making both variables constexpr.
}


void use1 (const string & value)
{
  // Which will print address and which will print the value?
  cout << &value << endl; // Prints the address.
  cout << value << endl; // Prints the value.
}

// Cpp Core Guidelines
// F.7: For general use, take T* or T& arguments rather than smart pointers.
void use2 (const shared_ptr<string> & value)
{
  // Which will print address and which will print the value?
  cout << value << endl; // Prints the address.
  cout << *value << endl; // Prints the value.
}

void use3 (string * value)
{
  // Which will print address and which will print the value?
  cout << value << endl; // Prints the address.
  cout << *value << endl; // Prints the value.
}

void print4 ()
{
  string value1 { "val1" };
  shared_ptr<string> value2 = make_shared<string> ("val2");
  string * value3 = new string ("val3"); // This will leak memory.
  use1 (value1);
  use2 (value2);
  use3 (value3);
}

class area
{
public:
  void set_length (int length);
  void set_width (int width);
  int get_area ();
  // Cpp Core Guidelines: C.9: Minimize exposure of members.
  // Better make the member variables private where possible.
  // Also: Always initialize variables, use initialization lists for member variables.
  int length;
  int width;
};

class locks
{
public:
  mutex mutex1 {};
  mutex mutex2 {};
};

void print5 ()
{
  locks foo;
  try {
    // dynamic_cast / static_cast / reinterpret_cast / const_cast ?
    char * bar = const_cast <char *> ("bar");
    // Safe: On exception, it will unlock.
    lock_guard<mutex> lock1 (foo.mutex1);
    // Unsafe: On exception, it will remain locked.
    foo.mutex2.lock ();
    // This will print garbage or else throw an exception.
    cout << (bar - 4) << endl;
    // Code might never reach this.
    foo.mutex2.unlock ();
  } catch (const exception & exception) {
    cout << exception.what();
  }
}

void print6 ()
{
  // Which method of passing variables is better in which circumstances?
  // F.16: For “in” parameters, pass cheaply-copied types by value and others by reference to const.
  // void f1 (const string& s);    // OK: pass by reference to const; always cheap.
  // void f2 (string s);           // Bad: potentially expensive.
  // void f3 (int x);              // OK: Unbeatable.
  // void f4 (const int& x);       // Bad: overhead on access in f4().
  // void f5 (unique_ptr<string>); // Input only, and moves ownership of the object.
  // F.17: For “in-out” parameters, pass by reference to non-const.
  // void f6 (string & s);         // This suggests an input/output variable.
  // F.18: For “will-move-from” parameters, pass by X&& and std::move the parameter.
  // void f7 (string && s);        // This suggests that the variable will be moved in f7.
  // F.20: For “out” output values, prefer return values to output parameters
  // vector<string> f8 ();
  
}


void print7 ()
{
  vector <int> values { 1, 2, 3 };
  // Cpp Core Guidelines
  // ES.1: Prefer the standard library to other libraries and to “handcrafted code”.
  auto sum1 = accumulate(begin(values), end(values), 0.0);
  int sum2 { 0 };
  {
    int max = values.size();   // bad: verbose, purpose unstated
    for (int i = 0; i < max; ++i) sum2 = sum2 + values[i];
  }
  cout << sum1 << " " << sum2 << endl;
}


void print8 ()
{
  // Cpp Core Guidelines.
  // ES.3: Don’t repeat yourself, avoid redundant code.
  
  bool flag { false };
  int x = 1, y = 2, z = 3;
  
  // Code duplication.
  if (flag) {
    cout << x << endl;
    cout << y << endl;
  }
  else {
    cout << x << endl;
    cout << z << endl;
  }
  
  // No duplication.
  cout << x << endl;
  if (flag)
    cout << y << endl;
  else
    cout << z << endl;
}
