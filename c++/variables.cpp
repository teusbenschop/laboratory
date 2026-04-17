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


#include "variables.h"

#include <cassert>
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

namespace variables {


namespace forward_like {

// https://en.cppreference.com/w/cpp/utility/forward_like.html
// Returns a reference to x which has similar properties to T&&.

struct TypeTeller
{
    void operator()(this auto&& self)
    {
        using self_type = decltype(self);
        using unref_self_type = std::remove_reference_t<self_type>;
        if constexpr (std::is_lvalue_reference_v<self_type>)
        {
            if constexpr (std::is_const_v<unref_self_type>)
                std::cout << "const lvalue" << std::endl;
            else
                std::cout << "mutable lvalue" << std::endl;
        }
        if constexpr (std::is_rvalue_reference_v<self_type>) {
            if constexpr (std::is_const_v<unref_self_type>)
                std::cout << "const rvalue" << std::endl;
            else
                std::cout << "mutable rvalue" << std::endl;
        }
    }
};


struct FarStates
{
    std::unique_ptr<TypeTeller> pointer;
    std::optional<TypeTeller> optional;
    std::vector<TypeTeller> container;

    auto&& from_opt(this auto&& self)
    {
        return std::forward_like<decltype(self)>(self.optional.value());
        // It is OK to use std::forward<decltype(self)>(self).opt.value(),
        // because std::optional provides suitable accessors.
    }

    auto&& operator[](this auto&& self, std::size_t i)
    {
        return std::forward_like<decltype(self)>(self.container.at(i));
        // It is not so good to use std::forward<decltype(self)>(self)[i], because
        // containers do not provide rvalue subscript access, although they could.
    }

    auto&& from_ptr(this auto&& self)
    {
        if (!self.pointer)
            throw std::bad_optional_access{};
        return std::forward_like<decltype(self)>(*self.pointer);
        // It is not good to use *std::forward<decltype(self)>(self).ptr, because
        // std::unique_ptr<TypeTeller> always dereferences to a non-const lvalue.
    }
};

void demo()
{
    return;

    FarStates my_state
    {
        .pointer{std::make_unique<TypeTeller>()},
        .optional{std::in_place, TypeTeller{}},
        .container{std::vector<TypeTeller>(1)},
      };

    my_state.from_ptr()();
    my_state.from_opt()();
    my_state[0]();

    std::cout << std::endl;

    std::as_const(my_state).from_ptr()();
    std::as_const(my_state).from_opt()();
    std::as_const(my_state)[0]();

    std::cout << std::endl;

    std::move(my_state).from_ptr()();
    std::move(my_state).from_opt()();
    std::move(my_state)[0]();

    std::cout << std::endl;

    std::move(std::as_const(my_state)).from_ptr()();
    std::move(std::as_const(my_state)).from_opt()();
    std::move(std::as_const(my_state))[0]();
}
}


namespace piecewise_construct {

enum Construction { none, from_tuple, from_int_float };

struct Foo
{
    Construction construction {none};

    // Constructor from a tuple.
    constexpr explicit Foo(std::tuple<int, float>)
    {
        construction = from_tuple;
    }

    // Constructor from an int and a float.
    constexpr explicit Foo(int, float)
    {
        construction = from_int_float;
    }
};


constexpr std::tuple<int, float> tuple(1, 1.0f);

// This creates a Foo object from a tuple.
constexpr std::pair<Foo, Foo> p1{tuple, tuple};
static_assert(p1.first.construction == Construction::from_tuple);
static_assert(p1.second.construction == Construction::from_tuple);

// This creates the same as above but then piecewise, so from an int and a float.
constexpr std::pair<Foo, Foo> p2{std::piecewise_construct, tuple, tuple};
static_assert(p2.first.construction == Construction::from_int_float);
static_assert(p2.second.construction == Construction::from_int_float);


void demo()
{
}
}


namespace forward_as_tuple {
// The helper "forward_as_tuple" takes a variadic pack of arguments
// and creates a tuple out of them.
// It determines the types of the elements of the arguments.
// - If it receives a lvalue then it will have a lvalue reference.
// - If it receives a rvalue then it will have a rvalue reference.
void demo()
{
    // The function returns a rvalue.
    auto rvalue = [] -> std::string
    {
        return "100";
    };

    // The variable i is a lvalue.
    int lvalue = 100;

    [[maybe_unused]] auto tuple = std::forward_as_tuple(lvalue, rvalue());

    // A lvalue reference binds to a lvalue. Marked with one ampersand (&).
    // A rvalue reference binds to a rvalue. Marked with two ampersands (&&).
    static_assert(std::is_same_v<decltype(tuple), std::tuple<int&, std::string&&>>);
}
}


namespace auto_x_decay_copy {
void demo()
{
    // A decay-copy is a copy of a variable which has lost some properties.
    // How does auto(x) help?
    // It is an easy way to make a copy of a variable.
    // It clearly communicates that it makes a copy of a variable.

    const auto pop_front = [] (auto& container) {
        std::erase(container, auto(container.front())); // <- Make copy through auto(x)
    };

    std::vector vector {1, 2, 3};
    assert (vector.size() == 3);
    pop_front (vector);
    assert (vector.size() == 2);
    pop_front (vector);
    assert (vector.size() == 1);
}
}



namespace aggregate_initialization {
// https://en.cppreference.com/w/cpp/language/aggregate_initialization

struct S
{
    int x{};
    struct Foo
    {
        int i{};
        int j{};
        int a[3];
    } foo;
};

// Using direct-list-initialization syntax.
constexpr S s1 =
{
    1,
    {
        2, 3,
        { 4, 5, 6 },
    }
};
static_assert(s1.x == 1);
static_assert(s1.foo.i == 2);
static_assert(s1.foo.j == 3);
static_assert(s1.foo.a[0] == 4);
static_assert(s1.foo.a[1] == 5);
static_assert(s1.foo.a[2] == 6);

// Same, but with brace elision.
// The compiler suggest braces around subobject initialization, rightly so.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
constexpr S s2 = {1, 2, 3, 4, 5, 6};
#pragma GCC diagnostic pop
static_assert(s2.x == 1);
static_assert(s2.foo.i == 2);
static_assert(s2.foo.j == 3);
static_assert(s2.foo.a[0] == 4);
static_assert(s2.foo.a[1] == 5);
static_assert(s2.foo.a[2] == 6);

// Brace elision only allowed with equals sign
constexpr int ar[] = {1, 2, 3}; // ar is int[3]
static_assert(ar[0] == 1);
static_assert(ar[1] == 2);
static_assert(ar[2] == 3);

// Too many initializer clauses
// char cr[3] = {'a', 'b', 'c', 'd'};

// Array initialized as {'a', '\0', '\0'}
constexpr char cr[3] = {'a'};
static_assert(cr[0] == 'a');
static_assert(cr[1] == '\0');
static_assert(cr[2] == '\0');

// Fully-braced 2D array: {1, 2}, {3, 4}.
constexpr int ar2d1[2][2] = {{1, 2}, {3, 4}};
static_assert(ar2d1[0][0] == 1);
static_assert(ar2d1[0][1] == 2);
static_assert(ar2d1[1][0] == 3);
static_assert(ar2d1[1][1] == 4);

// Compiler suggests braces around subobject.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
constexpr int ar2d2[2][2] = {3, 4};
#pragma GCC diagnostic pop
static_assert(ar2d2[0][0] == 3);
static_assert(ar2d2[0][1] == 4);
static_assert(ar2d2[1][0] == 0);
static_assert(ar2d2[1][1] == 0);

// Brace elision.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
constexpr int ar2d3[2][2] = {1, 2, 3, 4}; // brace elision: {1, 2}, {3, 4}.
#pragma GCC diagnostic pop
static_assert(ar2d3[0][0] == 1);
static_assert(ar2d3[0][1] == 2);
static_assert(ar2d3[1][0] == 3);
static_assert(ar2d3[1][1] == 4);

// Only first column: {1, 0}
constexpr int ar2d4[2][2] = {{1}, {2}};
static_assert(ar2d4[0][0] == 1);
static_assert(ar2d4[0][1] == 0);
static_assert(ar2d4[1][0] == 2);
static_assert(ar2d4[1][1] == 0);

constexpr std::array<int, 3> std_ar2{{1, 2, 3}};  // std::array is an aggregate
static_assert(std_ar2[0] == 1);
static_assert(std_ar2[1] == 2);
static_assert(std_ar2[2] == 3);

constexpr std::array<int, 3> std_ar1 = {1, 2, 3}; // brace-elision okay
static_assert(std_ar1[0] == 1);
static_assert(std_ar1[1] == 2);
static_assert(std_ar1[2] == 3);

// int a[] = {1, 2.0}; // narrowing conversion from double to int:
// error in C++11, okay in C++03

std::string ars[] = {
    std::string("one"),        // copy-initialization
    "two",                    // conversion, then copy-initialization
    {'t', 'h', 'r', 'e', 'e'} // list-initialization
};

union U
{
    int a;
    const char* b;
};
constexpr U u1 = {1};   // OK, first member of the union
static_assert(u1.a == 1);
// U u2 = {0, "asdf"}; // error: too many initializers for union
// U u3 = {"asdf"};    // error: invalid conversion to int

void demo()
{
}
}




void demo()
{
    forward_like::demo();
    piecewise_construct::demo();
    forward_as_tuple::demo();
    auto_x_decay_copy::demo();
    aggregate_initialization::demo();
}



}
