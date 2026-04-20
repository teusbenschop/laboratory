/*
Copyright (©) 2021-2026 Teus Benschop.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include "text.h"

#include <cassert>
#include <format>
#include <string>
#include <sstream>


namespace text {

namespace character_literals {
// Consistent character literal encoding.
// ReSharper disable once CppIdenticalOperandsInBinaryExpression
static_assert('A' == '\x41');
static_assert('A' == 0x41);
}


namespace escape_sequences {

void demo() {
    // Named universal character escapes.
    // https://www.unicode.org/Public/14.0.0/ucd/NamesList.txt
    assert("\N{CAT FACE}"      == std::string("🐱"));
    assert("\N{COW FACE}"      == std::string("🐮"));
    assert("\N{NATIONAL PARK}" == std::string("🏞"));

    // Delimited escape sequences.
    assert("\o{111}"  == std::string("I"));
    assert("\x{A0}"   != std::string(""));
    assert("\u{CAFE}" == std::string("쫾"));

}

}


namespace formatting_library {
// https://en.cppreference.com/w/cpp/utility/format
void demo()
{
    std::string result = std::format("A={} B={} C={}", "a", std::string("b"), 3);
    assert(result == "A=a B=b C=3");

    // Formats to an output iterator.
    std::string buffer;
    std::format_to
    (
        std::back_inserter(buffer), // the output iterator.
        "Hello, C++{}!", // the format string.
        20 // the argument(s).
    );
    assert(buffer == "Hello, C++20!");
}
}


namespace stream_manipulation {
void demo()
{
    {
        std::stringstream oss;
        oss << std::boolalpha << false << " " << std::noboolalpha << true;
        assert(oss.str() == "false 1");
    }
}
}


void demo() {
    escape_sequences::demo();
    formatting_library::demo();
    stream_manipulation::demo();
}


}

