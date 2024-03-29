#include <codecvt>
#include <type_traits>
#include <string>
#include <set>
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <vector>
#include <cstddef>
#include <bitset>
#include <sstream>
#include <istream>
using namespace std;
#include "main.h"


#define DIRECTORY_SEPARATOR "/"
//#define DIRECTORY_SEPARATOR "\\"

// C++ replacement for the dirname function, see http://linux.die.net/man/3/dirname.
// The BSD dirname is not thread-safe, see the implementation notes on $ man 3 dirname.
string filter_url_dirname_internal (string url, const char * separator)
{
  if (!url.empty ()) {
    if (url.find_last_of (separator) == url.length () - 1) {
      // Remove trailing slash.
      url = url.substr (0, url.length () - 1);
    }
    size_t pos = url.find_last_of (separator);
    if (pos != string::npos) url = url.substr (0, pos);
    else url = "";
  }
  if (url.empty ()) url = ".";
  return url;
}


// Dirname routine for the operating system.
// It uses the defined slash as the separator.
string filter_url_dirname (string url)
{
  return filter_url_dirname_internal (url, DIRECTORY_SEPARATOR);
}


//struct cpp17_struct {
//    mutable int x1 : 2;
//    mutable string y1;
//};

//cpp17_struct cpp17_function()
//{
//  return cpp17_struct {1, "2.3"};
//}

#pragma clang ignored "-Wxxx"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
//  {
//    set <string> myset {"hello"};
//    int inserted {0}, skipped {0};
//    for (int i {2}; i; --i) {
//      if (auto [iter, success] = myset.insert("Hello"); success) {
//        if (*iter != "Hello") cout << "Inserted: " << " " << *iter << endl;
//        inserted++;
//      }
//      else {
//        skipped++;
//      }
//    }
//    if (inserted != 1) cout << "Inserted: " << inserted << endl;
//    if (skipped != 1) cout << "Skipped: " << skipped << endl;
//  }

//  {
//    struct fields {
//      int b {1}, d {2}, p {3}, q {4};
//    };
//    {
//      [[maybe_unused]] const auto [b, d, p, q] = fields{};
//      // cout << b << " " << d << " " << p << " " << q << endl;
//      // 1 2 3 4
//    }
//    {
//      [[maybe_unused]] const auto [b, d, p, q] = []
//      {
//        return fields{4, 3, 2, 1};
//      } ();
//      // cout << b << " " << d << " " << p << " " << q << endl;
//      // 4 3 2 1
//    }
//  }

//  {
//    struct cpp17_struct {
//      mutable int x1 : 2;
//      mutable string y1;
//    };
//    const auto [x, y] = []
//    {
//      return cpp17_struct {1, "1"};
//    } ();
//  }

//  {
//    const auto [x, y] = cpp17_function();
//    // x is an int lvalue identifying the 2-bit bit field
//    // y is a const volatile double lvalue
//    x = -2;
//    y = "-2.3";
//  }

//  {
//    static_assert(true);
//  }

//  {
//    [[maybe_unused]] auto x1 = { 1, 2 };
//    [[maybe_unused]] auto x2 = { 1.0, 2.0 };
//    [[maybe_unused]] auto x3 { 2 };
//    [[maybe_unused]] auto x4 = { 3 };
//
//    float x {1.1};
//    char  y {'a'};
//    int   z {1};
//    tuple<float&, char&&, int> tpl (x, move(y), z);
//    [[maybe_unused]] const auto& [a,b,c] = tpl;
//    // a names a structured binding that refers to x; decltype(a) is float&
//    //cout << x << " " << a << endl;
//    // b names a structured binding that refers to y; decltype(b) is char&&
//    //cout << y << " " << b << endl;
//    // c names a structured binding that refers to the 3rd element of tpl; decltype(c) is const int
//    //cout << z << " " << c << endl;
//  }
  
  // Initializers in "if" and "switch" statements.
//  {
//    set <string> myset {};
//    if (auto [iter, success] = myset.insert("hello"); success) {
//      if (*iter != "hello") cout << "Inserted: " << " " << *iter << endl;
//    }
//    switch (auto i = 1; i) {
//      case 0:
//        break;
//      case 1:
//        break;
//      default:
//        break;
//    }
//  }
  
//  {
//    string url = "/var/log/app";
//    string dirname = filter_url_dirname (url);
//    cout << dirname << endl;
//    filesystem::path p (url);
//    dirname = p.parent_path().string();
//    cout << dirname << endl;
//  }

//  {
//    string url = R"(C:\var\log\app)";
//    string dirname = filter_url_dirname (url);
//    filesystem::path p (url);
//    dirname = p.parent_path ().string ();
//    cout << dirname << endl;
//    cout << filesystem::path::preferred_separator << endl;
//  }
  
//  {
//    try {
//      filesystem::path p ("log");
//      filesystem::remove (p);
//    } catch (exception ex) {
//      cout << ex.what() << endl;
//    }
//  }

//  {
//    try {
//      filesystem::path path ("/tmp/hi.txt");
//      bool exists = filesystem::exists (path);
//      cout << "exists: " << exists << endl;
//    } catch (exception ex) {
//      cout << ex.what() << endl;
//    }
//  }

//  {
//    try {
//      filesystem::path path (R"(C:\bibledit)");
//      for (auto const & directory_entry : filesystem::directory_iterator {path})
//      {
//        filesystem::path path = directory_entry.path();
//        cout << path.string() << endl;
//      }
//    } catch (exception ex) {
//      cout << ex.what() << endl;
//    }
//  }

//  {
//    // Quotes in ordinary string literal.
//    cout << "'hello world'" << endl;
//    // Quotes in raw string literal. Example: R"(text)".
//    cout << R"("hello world")" << endl;
//    // Clean way to insert quotes.
//    cout << std::quoted ("hello world") << endl;
//  }
  
//  {
//    for (int i = 1; i < 10; i++) {
//      int rev_code = i;
//      cout << rev_code;
//      rev_code >>= 1;
//      cout << " " << rev_code;
//      int tree_cur = 10;
//      //tree_cur -= ((rev_code >>= 1) & 1);
//      tree_cur -= ((rev_code) & 1);
//      cout << " " << tree_cur << endl;
//    }
//  }

  {
    // Example "03 02 01 00 00"
    using OctetStream = vector <unsigned char>;
    vector <unsigned char> characters { 3, 2, 1, 0, 0 };
    vector <byte> bytes { byte{3}, byte{2}, byte{1}, byte{0}, byte{0} };

    cout << static_cast <int> (characters[0]) << endl;
    cout << to_integer<int>(bytes[0]) << endl;

    cout << "Should be 513:" << endl;
    cout << (static_cast <int> (characters[1]) << 8) + static_cast <int> (characters[2]) << endl;
    cout << (to_integer<int>(bytes[1]) << 8) + to_integer<int>(bytes[2]) << endl;

    
    
    int number1 {0};
    {
      stringstream ss;
      ss << 256 * static_cast<int>(characters[1]) + static_cast<int>(characters[2]);
      ss >> std::dec >> number1;
    }
    cout << number1 << endl;

    
  }
  
  
  return EXIT_SUCCESS;
}

