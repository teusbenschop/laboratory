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

#include "alignment.h"

#include <string>

namespace designated_initializers {
struct S2;
}

namespace alignment {

static_assert(sizeof(bool) == 1);
static_assert(sizeof(char) == 1);
static_assert(sizeof(int) == 4);
static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);

// Each member of the struct begins on its natural size.
// The entire struct is padded till the size of the largest member.

void demo()
{
    struct S1
    {
        bool b; // 1 byte
        // 3 bytes padding
        int i; // 4 bytes
    };
    static_assert(sizeof(S1) == 8);
    static_assert(alignof(S1) == 4);

    struct S2
    {
        double d; // 8 bytes
        bool b; // 1 byte (+ 7 padding)
    };
    static_assert(sizeof(S2) == 16);
    static_assert(alignof(S2) == 8);

    struct S3
    {
        int i1; // 4 bytes (+ 4 padding)
        double d; // 8 bytes
        int i2; // 4 bytes (+ 4 padding)
    };
    static_assert(sizeof(S3) == 24);
    static_assert(alignof(S3) == 8);

    struct S4
    {
        int i1; // 4 bytes
        int i2; // 4 bytes
        double d; // 8 bytes
    };
    static_assert(sizeof(S4) == 16);
    static_assert(alignof(S4) == 8);

    struct S5
    {
        double d; // 8 bytes
        bool b1; // 1 byte
        // 3 bytes padding
        int i; // 4 bytes
        bool b2; // 1 byte
        // 7 bytes padding
    };
    static_assert(sizeof(S5) == 24);
    static_assert(alignof(S5) == 8);

    struct S6
    {
        bool b1;
        bool b2;
    };
    static_assert(sizeof(S6) == 2);
    static_assert(alignof(S6) == 1);
}
}
