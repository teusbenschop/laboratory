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


#include "constraints.h"

#include <iostream>
#include <string>


// https://en.cppreference.com/w/cpp/language/constraints
// Class templates, function templates, and non-template functions
// (typically members of class templates)
// may be associated with a constraint,
// which specifies the requirements on template arguments,
// which can be used to select the most appropriate function overloads and template specializations.
// Named sets of such requirements are called concepts.
// Each concept is a predicate, evaluated at compile time,
// and becomes a part of the interface of a template where it is used as a constraint.


namespace unconstrained_errors {

// An unconstrained template function.
template <typename T>
constexpr auto basic_plus(const T& x, const T& y)
{
    return x + y;
}

// Call the template with numbers: OK.
static_assert(basic_plus(1, 2) == 3);

// Call the template with strings:
// Oops, it works, but not as intended, it concatenates instead of summing.
// It would help if the passed types could be constrained.
// ReSharper disable once CppEvaluationInternalFailure
static_assert(basic_plus(std::string("a"), std::string("b")) == "ab");

// Call the template with chars:
// OK, it fails:
// invalid operands of types 'const char*' and 'const char*' to binary 'operator+'
// static_assert(basic_sum("a", "b") == "?");

void demo()
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

struct [[maybe_unused]] Unrelated
{
    static constexpr int value{10};
};

template <typename T>
// This template has a constraint to make sure the correct type is passed.
// The template function requires that the parameter type is derived from the Base struct.
    requires std::derived_from<T, Base>
constexpr int get_value(const T& object)
{
    return object.value;
}

static_assert(get_value(Base()) == 10); // This compiles.
static_assert(get_value(Derived()) == 10); // This compiles.
// static_assert(get_value(Unrelated()) == 10);
// The above fails to compile due to the constraint:
// error: no matching function for call to 'get_value' ... and so on, multiple diagnostic errors.


void demo()
{

}
}


namespace demonstrate_constraints {

// Define a basic concept.
template <typename T>
concept floating_point = std::is_floating_point_v<T>;

// Define a concept consisting of another concept or a type trait.
template <typename T>
concept number = floating_point<T> or std::is_integral_v<T>;

// Define a concept that requires the type to have the given iterators.
template <typename T>
// ReSharper disable once CppConceptNeverUsed
concept range = requires(T& t)
{
    std::begin(t);
    std::end(t);
};

// Constraining a type with a concept.
template <typename T>
    requires number<T>
constexpr auto add_both_template(T a, T b)
{
    return a + b;
}

static_assert(add_both_template(10, 11) == 21);

// Using concepts ("number") with abbreviated function templates.
constexpr number auto add_both_abbreviated(number auto a, number auto b)
{
    return a + b;
}

static_assert(add_both_abbreviated(10, 11) == 21);

// Declaration of the concept "hashable", which is satisfied by any type 'T'
// such that for values 'a' of type 'T', the expression std::hash<T>{}(a)
// compiles and its result is convertible to std::size_t
template <typename T>
concept hashable = requires(T a)
{
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

// Constrained C++20 function template.
template <hashable T>
void constrained_func1(T)
{
}

// Alternative ways to apply the same constraint:
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
template <typename P>
concept HasStaticMethod = requires {
    { P::method_name() };
    // The compiler attempts to instantiate this call at compile-time.
    // If the function isn't static (or doesn't exist), this fails.
};

// A variadic template function takes a variable number of arguments.
template <typename T, typename... Args>
void variadic_template_print(T t, Args... args)
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
void variadic_abbreviated_print(auto t, auto... args)
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
T generic_mod_overload(T v, T n)
{
    // Integral version.
    return v % n;
}

template <std::floating_point T>
T generic_mod_overload(T v, T n)
{
    // Floating point version.
    return std::fmod(v, n);
}

void demo()
{

}
}

