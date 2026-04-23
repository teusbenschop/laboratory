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
#include <functional>
#include <iostream>
#include <type_traits>
#include <utility>
#include <variant>
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


namespace variant {
void demo()
{
    // Initialized with the monostate as that is the first variant.
    std::variant<std::monostate, int, float> variant;

    // Getting the wrong variant throws a bad_variant_access exception.
    try
    {
        std::get<int>(variant);
        assert(false);
    }
    catch (const std::bad_variant_access& e)
    {
        assert(true);
    }

    variant = 10;
    assert(std::get<int>(variant) == 10);
    assert(std::get<1>(variant) == 10); // Same as above.
    assert(std::holds_alternative<int>(variant));
}
}


namespace designated_initializers {
// https://en.cppreference.com/w/cpp/language/aggregate_initialization#Designated_initializers

struct S1 {
    const int x{};
    const int y{};
    const int z{};
};

// Fails because designator order does not match declaration order.
//constexpr S1 s1 {.y = 2, .x = 1};

// OK: s2.y initialized to 0.
constexpr S1 s2 {.x = 1, .z = 2};
static_assert(s2.x == 1);
static_assert(s2.y == 0);
static_assert(s2.z == 2);

struct S2 {
    const int o {33};
    const int n {42};
    const int m {-1};
};
constexpr S2 s3 {.m = 21};
// Initializes o with 33.
// Then initializes n with = 42.
// Then initializes m with = 21.
static_assert(s3.o == 33);
static_assert(s3.n == 42);
static_assert(s3.m == 21);

void demo()
{
}
}



namespace mathematical_functions {
void demo()
{
    assert(std::ceil(1.2f) == 2);
}
}


namespace reference_wrappers {
void demo()
{
    const auto fn = [](int& n1, int& n2, [[maybe_unused]] const int& n3)
    {
        n1++; // Increases the copy of n1 stored in the function object.
        n2++; // Increases the caller's n2.
        // n3++; Compile error: cannot assign to variable 'n3' with const-qualified type 'const int &'
    };

    int n1{1};
    int n2{2};
    int n3{3};

    std::function<void()> bound_fn = std::bind(fn, n1, std::ref(n2), std::cref(n3));

    bound_fn();

    assert(n1 == 1); // This is left unchanged, because it was passed by value to the function.
    assert(n2 == 3); // This was passed by reference, and got increased by the function.
    assert(n3 == 3); // Passed by const reference, could not get increased.
}
}



namespace enable_shared_from_this {
// The std::enable_shared_from_this allows an object
// that is currently managed by a std::shared_ptr
// to safely generate additional std::shared_ptr instances, pt1, pt2 etc.,
// that all share ownership of the object with the original shared_ptr.
void demo()
{
    struct Struct : public std::enable_shared_from_this<Struct>
    {
        std::shared_ptr<Struct> getptr()
        {
            return shared_from_this();
        }
    };

    // The original and derived shared pointers share the same object.
    {
        std::shared_ptr<Struct> object1 = std::make_shared<Struct>();
        std::shared_ptr<Struct> object2 = object1->getptr();
        assert(object1 == object2);
        assert(object1.use_count() == 2);
        assert(object2.use_count() == 2);
    }

    // Bad use: shared_from_this is called without having std::shared_ptr owning the caller.
    // It will throw an exception.
    try
    {
        Struct object1;
        std::shared_ptr<Struct> object2 = object1.getptr();
        assert(false); // It should never arrive here.
    }
    catch (std::bad_weak_ptr& e)
    {
        assert(e.what() == std::string("bad_weak_ptr"));
    }
}
}


namespace swapping_and_exchanging {
void demo()
{
    {
        // Swap values in a and b.
        int a = 1, b = 2;
        std::swap(a, b);
        assert(a == 2);
        assert(b == 1);
    }
    {
        int a = 1;
        // Put a new value in a, and return the old value.
        const int b = std::exchange(a, 2);
        assert(a == 2);
        assert(b == 1);
    }
}
}


namespace initializer_list {
// The initializer_list is a light-weight proxy object that gives access to the backing array.
template <typename T>
struct Struct
{
    std::vector<T> vec;

    Struct(std::initializer_list<T> il) : vec(il)
    {
    }

    void append(std::initializer_list<T> il)
    {
        vec.insert(vec.end(), il.begin(), il.end());
    }

    std::pair<const T*, std::size_t> c_arr() const
    {
        // Copy list-initialization in return statement.
        // Note this is NOT a use of std::initializer_list.
        return {std::addressof(vec.at(0)), vec.size()};
    }
};

template <typename T>
void templated_fn(T)
{
}

void demo()
{
    Struct<int> s = {1, 2, 3, 4, 5}; // copy list-initialization
    s.append({6, 7, 8}); // list-initialization in function call
    assert(s.c_arr().second == 8); // Assert the backing array size.
    assert(s.vec.size() == 8);

    // Range-for over brace-init-list
    for (int x : {-1, -2, -3}) // the rule for auto makes this ranged-for work
        assert(x == -3 or x == -2 or x == -1);

    auto il1 = {10, 11, 12}; // special rule for auto
    assert(il1.size() == 3);

    auto il_copy = il1; // a shallow-copy of top-level proxy object
    assert(il_copy.begin() == il1.begin()); // guaranteed: backing array is the same

    std::initializer_list<int> il2{-3, -2, -1};
    assert(il2.begin()[2] == -1); // note the replacement for absent operator[]
    il2 = il1; // shallow-copy
    assert(il2.begin() == il1.begin()); // guaranteed

    // templated_fn({1, 2, 3}); // compiler error! "{1, 2, 3}" is not an expression.
    // It has no type, and so T cannot be deduced.
    templated_fn<std::initializer_list<int>>({1, 2, 3}); // OK
    templated_fn<std::vector<int>>({1, 2, 3}); // Also OK
}
}



void demo()
{
    forward_like::demo();
    piecewise_construct::demo();
    forward_as_tuple::demo();
    auto_x_decay_copy::demo();
    aggregate_initialization::demo();
    variant::demo();
    designated_initializers::demo();
    mathematical_functions::demo();
    reference_wrappers::demo();
    enable_shared_from_this::demo();
    swapping_and_exchanging::demo();
    initializer_list::demo();
}



}
