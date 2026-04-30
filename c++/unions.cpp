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

#include "unions.h"

#include <cassert>
#include <iostream>
#include <ostream>

namespace unions {

// A union is a special class that holds only one of its non-static members at once.

union U1
{
    int i; // occupies 4 bytes.
    float f; // occupies 4 bytes.
    char c; // occupies 1 byte.
}; // Total size = 4 bytes (the largest member).

void demo()
{
    // Initialize the first member, which is i.
    U1 u1 {10};
    assert(u1.i == 10);
    // Change the active member to the second, the f.
    u1.f = 1.0f;
    assert(u1.f == 1.0f);
    // Reading from non-active member is undefined.
    assert(u1.i != 10);
    // Change the active member to the last, the c.
    u1.c = 'a';
}

}
