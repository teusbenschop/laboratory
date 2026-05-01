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
#include <vector>

namespace unions {

// A union is a special class that holds only one of its non-static members at any time.

static std::string indicator;

struct StructWithConstructorAndDestructor {
    StructWithConstructorAndDestructor() {
        indicator = "construct";
    }
    ~StructWithConstructorAndDestructor() {
        indicator = "destruct";
    }
    int value {10};
};

void demo()
{
    {
        union Union
        {
            int i; // occupies 4 bytes.
            float f; // occupies 4 bytes.
            char c; // occupies 1 byte.
        };
        // Total union size = 4 bytes (the largest member).
        static_assert(sizeof(Union) == 4);

        // Initialize the first member, which is i.
        Union bu {10};
        assert(bu.i == 10);
        // Change the active member to the second, the f.
        bu.f = 1.0f;
        assert(bu.f == 1.0f);
        // Reading from non-active member is undefined.
        assert(bu.i != 10);
        // Change the active member to the last, the c.
        bu.c = 'a';
    }

    {
        union ConstructDestructUnion {
            // Members with defined constructor / destructors require constructor and destructor calls to use them.
            std::string s;
            std::vector<int> v;
            StructWithConstructorAndDestructor cds;
            // Needs to know which member is active to destroy it.
            ~ConstructDestructUnion() {}
        };
        static_assert(sizeof(ConstructDestructUnion) == std::max(sizeof(int), sizeof(std::vector<int>)));

        // Construct the union and the string member.
        ConstructDestructUnion cdu {"hello"};
        assert(cdu.s == "hello");
        // Reading from the vector is undefined, likely crashes.

        // Switching the active member from string to vector requires two steps:
        // 1: Destroy the string. 2: Construct the vector at correct address.
        cdu.s.~basic_string();
        new (std::addressof(cdu.v)) std::vector<int>;

        // The vector is the active member now.
        cdu.v.emplace_back(10);
        assert(cdu.v.size() == 1);
        assert(cdu.v.front() == 10);

        // Switch again.
        cdu.v.~vector<int>();
        new (std::addressof(cdu.cds)) StructWithConstructorAndDestructor();
        assert(cdu.cds.value == 10);
        // Struct out of scope: Oops, user defined destructor not called: Call it manually.
        cdu.cds.~StructWithConstructorAndDestructor();
    }
    assert(indicator == "destruct");

    // Union member lifetime begins when it is made active.
    // If another member was active, its lifetime ends.
    {
        union A { int x; int y[4]; };
        struct B { A a; };
        union C { B b; int k; };
        C c; // Does not start lifetime of any member.
        c.b.a.y[3] = 4; // Starts lifetime of member y. All its parents are created too.
        assert(c.b.a.y[3] == 4);
    }

    {
        struct X { const int a; int b; };
        union Y { X x; int k; };
        Y y = {1,2}; // Member y.x is active member.
        assert(y.x.a == 1);
        y.k = 10; // Ends lifetime of y.x and starts lifetime of y.k.
        assert(y.x.a != 1);
        assert(y.k == 10);
        // Undefined behaviour. Clang on macOS stores the value in x.b and leaves k as is.
        y.x.b = 3;
        assert(y.k == 10);
        assert(y.x.b == 3);
    }
    {
        // Anonymous unions:
        // 1. Restrictions on members.
        // 2. Inject their members in the enclosing scope.
        union {
            int i;
            const char* ch;
        };
        i = 10;
        assert(i == 10);
        ch = "hello";
        assert(ch == "hello");
        assert(i != 10);
    }
    {
        // The union does not know which member is active.
        // The user needs to track that.
        // One way of tracking that is to create a tagged union.
        struct S {
            enum {has_char, has_int, has_float} tag;
            union {
                char c;
                int i;
                float f;
            };
        };
        // The struct keeps track of the active member via the tag.
        // The user still needs to set both the tag and the active member.
        S s;
        s.tag = S::has_char;
        s.c = 'c';
    }
    // The std::variant is a modern replacement of the union.

}

}
