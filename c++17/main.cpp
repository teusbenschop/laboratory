#include "main.h"
#include <codecvt>
#include <type_traits>
#include <string>
#include <set>
#include <iostream>

using namespace std;

struct cpp17_struct {
    mutable int x1 : 2;
    mutable string y1;
};

cpp17_struct cpp17_function()
{
  return cpp17_struct {1, "2.3"};
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
  
  {
    set <string> myset {"hello"};
    int inserted {0}, skipped {0};
    for (int i {2}; i; --i) {
      if (auto [iter, success] = myset.insert("Hello"); success) {
        if (*iter != "Hello") cout << "Inserted: " << " " << *iter << endl;
        inserted++;
      }
      else {
        skipped++;
      }
    }
    if (inserted != 1) cout << "Inserted: " << inserted << endl;
    if (skipped != 1) cout << "Skipped: " << skipped << endl;
  }

  {
    struct fields {
      int b {1}, d {2}, p {3}, q {4};
    };
    {
      [[maybe_unused]] const auto [b, d, p, q] = fields{};
      // cout << b << " " << d << " " << p << " " << q << endl;
      // 1 2 3 4
    }
    {
      [[maybe_unused]] const auto [b, d, p, q] = []
      {
        return fields{4, 3, 2, 1};
      } ();
      // cout << b << " " << d << " " << p << " " << q << endl;
      // 4 3 2 1
    }
  }

  {
    struct cpp17_struct {
      mutable int x1 : 2;
      mutable string y1;
    };
    const auto [x, y] = []
    {
      return cpp17_struct {1, "1"};
    } ();
  }

  {
    const auto [x, y] = cpp17_function();
    // x is an int lvalue identifying the 2-bit bit field
    // y is a const volatile double lvalue
    x = -2;
    y = "-2.3";
  }

  {
    static_assert(true);
  }

  {
    [[maybe_unused]] auto x1 = { 1, 2 };
    [[maybe_unused]] auto x2 = { 1.0, 2.0 };
    [[maybe_unused]] auto x3 { 2 };
    [[maybe_unused]] auto x4 = { 3 };
    
    float x {1.1};
    char  y {'a'};
    int   z {1};
    tuple<float&, char&&, int> tpl (x, move(y), z);
    [[maybe_unused]] const auto& [a,b,c] = tpl;
    // a names a structured binding that refers to x; decltype(a) is float&
    //cout << x << " " << a << endl;
    // b names a structured binding that refers to y; decltype(b) is char&&
    //cout << y << " " << b << endl;
    // c names a structured binding that refers to the 3rd element of tpl; decltype(c) is const int
    //cout << z << " " << c << endl;
  }
  
  // Initializers in "if" and "switch" statements.
  {
    set <string> myset {};
    if (auto [iter, success] = myset.insert("hello"); success) {
      if (*iter != "hello") cout << "Inserted: " << " " << *iter << endl;
    }
    switch (auto i = 1; i) {
      case 0:
        break;
      case 1:
        break;
      default:
        break;
    }
  }

  return 0;
}

