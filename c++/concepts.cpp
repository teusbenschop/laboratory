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


#include <cmath>
#include <iostream>
#include <list>
#include <string>
#include <ranges>
#include <vector>
#include "concepts.h"


namespace concepts {

// https://en.cppreference.com/w/cpp/language/constraints
// Class templates, function templates, and non-template functions
// (typically members of class templates)
// may be associated with a constraint,
// which specifies the requirements on template arguments,
// which can be used to select the most appropriate function overloads and template specializations.
// Named sets of such requirements are called concepts.
// Each concept is a predicate, evaluated at compile time,
// and becomes part of the interface of a template where it is used as a constraint.


namespace unconstrained_errors {

// An unconstrained template function.
template <typename T>
static constexpr auto plus(const T& a, const T& b)
{
    return a + b;
}

// Call the template with numbers: OK.
static_assert(plus(1, 2) == 3);

// Call the template with strings:
// Oops, it works, but not as intended, it concatenates instead of taking the sum.
// It would help if the passed types could be constrained.
static_assert(plus(std::string("1"), std::string("2")) == "12");

// Call the template with char pointers:
// OK, it fails:
// invalid operands of types 'const char*' and 'const char*' to binary 'operator+'
//static_assert(unconstrained_plus("a", "b") == "?");

static void demo()
{
}
}


namespace constraint_derived_from {

// Three classes, two related, one unrelated, to demonstrate constraints.
struct Base
{
    static constexpr int value{10};
};

struct Derived : Base
{
};

struct Unrelated
{
    static constexpr int value{10};
};

template <typename T>
// This template has a constraint to make sure the correct type is passed.
// The template function requires that the parameter type is derived from the Base struct.
    requires std::derived_from<T, Base>
static constexpr int get_value(const T& object)
{
    return object.value;
}

static_assert(get_value(Base()) == 10); // This compiles.
static_assert(get_value(Derived()) == 10); // This compiles.
//static_assert(get_value(Unrelated()) == 10);
// The above fails to compile due to the constraint:
// error: no matching function for call to 'get_value' ... and so on, multiple diagnostic errors.

static void demo()
{
}
}


namespace demonstrate_constraints {

// Define a basic concept.
template <typename T>
concept floating_point = std::is_floating_point_v<T>;

// Define a concept consisting of another concept and a type trait.
template <typename T>
concept number = floating_point<T> or std::is_integral_v<T>;

// Define a concept that requires the type to have the given iterators.
template <typename T>
concept range = requires(T& t)
{
    t.begin();
    t.end();
};

// Constraining a type with a concept.
template <typename T>
    requires number<T>
static constexpr auto add_both_template(const T& a, const T& b)
{
    return a + b;
}

static_assert(add_both_template(10, 11) == 21);

// Using concepts ("number") with abbreviated function templates.
static constexpr number auto add_both_abbreviated(const number auto& a, const number auto& b)
{
    return a + b;
}

static_assert(add_both_abbreviated(10, 11) == 21);

// Declaration of the concept "hashable",
// which is satisfied by any type 'T' such that for values 't' of type 'T',
// the expression std::hash<T>{}(t) compiles and its result is convertible to std::size_t
template <typename T>
concept hashable = requires(T t)
{
    { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
};

// Constrained C++20 function template.
template <hashable T>
void constrained_func1(T)
{
}

// Alternative ways to apply the same constraint.
template <typename T>
    requires hashable<T>
void constrained_func2(T)
{
}

template <typename T>
void constrained_func3(T) requires hashable<T>
{
}

void constrained_func4(hashable auto a)
{
}


// A concept that tests whether a struct has a static member function.
template <typename T>
concept has_static_method = requires {
    { T::method_name() };
    // The compiler attempts to instantiate this call at compile-time.
    // If the function isn't static (or doesn't exist), this fails.
};


// A variadic template function takes a variable number of arguments.
template <typename T, typename... Args>
static void variadic_template_print(T t, Args... args)
{
    // The "if constexpr" is evaluated at compile time.
    // Here, if no arguments are passed, it no longer does recursion.
    if constexpr (not sizeof ...(args))
    {
        std::cout << t << std::endl;
    }
    else
    {
        std::cout << t << ", ";
        variadic_template_print(args...);
    }
}

// The same as above, now as an abbreviated variadic template function.
static void variadic_abbreviated_print(auto t, auto... args)
{
    if constexpr (not sizeof ...(args))
        std::cout << t << std::endl;
    else
    {
        std::cout << t << ", ";
        variadic_abbreviated_print(args...);
    }
}

// Use function overloading.
template <std::integral T>
static T generic_mod_overload(T v, T n)
{
    // Integral version.
    return v % n;
}

template <std::floating_point T>
static T generic_mod_overload(T v, T n)
{
    // Floating point version.
    return std::fmod(v, n);
}


// Concept for equality and comparability.
// The result (after ->) must be another concept.
template <typename T>
concept equality_comparable = requires (T& a, T& b)
{
    { a == b } -> std::convertible_to<bool>;
    { a != b } -> std::convertible_to<bool>;
};
static_assert(equality_comparable<std::string>);
namespace { struct no_compare {}; }
static_assert(not equality_comparable<no_compare>);

template<typename T1, typename T2 = T1>
concept equality_comparable_2_types = requires (T1& v1, T2& v2)
{
    { v1 == v2 } -> std::convertible_to<bool>;
    { v1 != v2 } -> std::convertible_to<bool>;
    { v2 == v1 } -> std::convertible_to<bool>;
    { v2 != v1 } -> std::convertible_to<bool>;
};
static_assert(equality_comparable_2_types<unsigned, int>);


// Concept for the element type from a range.
using range_value = std::ranges::range_value_t<std::vector<int>>;
static_assert(std::is_same_v<int, range_value>);


static void demo()
{
}
}


namespace concept_constrains_function_return_type {

template<typename T>
concept arithmetic = std::is_arithmetic_v<T>;

static arithmetic auto func(const int i)
{
    return i;
}

static void demo()
{
}
}


namespace standard_concepts {

namespace {
struct Base{};
struct Derived : Base {};
struct Unrelated { int i{}; };
}

static_assert(std::same_as<Base, Base>);
static_assert(std::derived_from<Derived, Base>);
static_assert(std::convertible_to<Derived, Base>);
static_assert(std::common_reference_with<Derived, Derived>); // shared a common reference type.
static_assert(std::common_with<Derived, Derived>); // share a common type.
static_assert(std::integral<char>);
static_assert(std::signed_integral<int>);
static_assert(std::unsigned_integral<unsigned>);
static_assert(std::floating_point<float>);
static_assert(std::assignable_from<int&, char>); // first type, an lvalue, can be assigned from second type.
static_assert(std::swappable_with<int&, int&>); // first type can be swapped with second type through std::ranges::swap.

using int_long = std::common_type_t<int, long>;
static_assert(std::same_as<int_long, long>);

static_assert(std::equality_comparable_with<int, char>); // types can be compared for equivalence through ==.
static_assert(std::totally_ordered_with<int, unsigned>); // comparing with < <= > >= gives total order.
static_assert(std::three_way_comparable_with<int, uint16_t>); // can compare with <=>.

static_assert(std::destructible<Base>);
static_assert(std::constructible_from<Unrelated, int>); // first type can be constructed from args...
static_assert(std::default_initializable<Base>);
static_assert(std::move_constructible<Base>);
static_assert(std::copy_constructible<Base>);
static_assert(std::movable<Base>);
static_assert(std::copyable<Base>);

struct input_or_output_iter
{
    using difference_type = std::ptrdiff_t;
    int operator*();
    input_or_output_iter& operator++();
    void operator++(int) {++*this;}
};
// The iterator must have:
// 1. Dereferencing through operator*
// 2. Incrementing through operator++
static_assert(std::input_or_output_iterator<input_or_output_iter>);

struct input_iter
{
    using difference_type = std::ptrdiff_t;
    using value_type = int;
    int operator*() const;
    input_iter& operator++();
    void operator++(int) {++*this;}
};
// Same as above plus referenced value must be readable.
static_assert(std::input_iterator<input_iter>);

using vector_int_iter = std::vector<int>::iterator;
static_assert(std::bidirectional_iterator<vector_int_iter>);

struct minimum_range
{
    int* begin();
    int* end();
};
static_assert(std::ranges::range<minimum_range>);

// Sized range knows its size in constant time.
static_assert(std::ranges::sized_range<std::list<int>>);

static constexpr unsigned fun1() { return 0; }
static_assert(std::is_same_v<decltype(fun1()), unsigned>);
static_assert(std::is_same_v<std::invoke_result_t<decltype(fun1)>, unsigned>);


static void demo()
{
}
}


void demo()
{
    unconstrained_errors::demo();
    constraint_derived_from::demo();
    demonstrate_constraints::demo();
    concept_constrains_function_return_type::demo();
    standard_concepts::demo();
}
}