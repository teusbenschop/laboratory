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


#include "characters.h"

#include <cassert>
#include <string>

namespace characters {
void demo() {

    // Consistent character literal encoding.
    // ReSharper disable once CppIdenticalOperandsInBinaryExpression
    static_assert('A' == '\x41');
    static_assert('A' == 0x41);

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
