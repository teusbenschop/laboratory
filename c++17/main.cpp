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



int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{




  

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

