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

// #import std;
#include <cassert>
#include <coroutine>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <type_traits>
#include <vector>
#include "templates.h"
#include "templates_explicit.h"

namespace templates {

// Template programming is about letting the compiler deal with the types.
// Template metaprogramming is about letting the compiler calculate values.

namespace default_type {

template <typename T = int>
static constexpr T sum(T a, T b) noexcept { return a + b; }

// Pass the type.
static_assert(std::is_same_v<decltype(sum<unsigned>(1, 2)), unsigned>);
// Omit the type: it takes int.
static_assert(std::is_same_v<decltype(sum<>(2, 3)), int>);
static_assert(std::is_same_v<decltype(sum  (4, 5)), int>);

static void demo()
{
}
}

namespace simple_function_template {

// General template.
template <typename T>
static T add (T a, T b) noexcept
{
    return a + b;
}

struct Struct { int v{}; };

// Specialized template.
template <> // This could be left out.
Struct add (const Struct a, const Struct b) noexcept
{
    return Struct{a.v + b.v };
}

static void demo()
{
    // Call template: Compiler instantiates template. This is implicit template instantiation.
    add(1,                2);
    add(1.0f,             2.0f);
    add(std::string("a"), std::string("b"));
    add(Struct(1),        Struct(2));
}
}

namespace class_template {

template <typename T>
struct Struct
{
    T a;
    Struct(T a) : a(a) {}
};

static void demo()
{
}
}


namespace non_type_template_parameter {
// A template placeholder of a constant value, called a non-type parameter.

template <int i>
float func (const float d)
{
    if constexpr (i == 10)
        return 100.0f;
    return d;
}

static void demo()
{
    assert (func<10>(0.0f) == 100.0f);
    assert (func<0>(10.0f) == 10.0f);
}
}

namespace variable_template {

// Basic variable template.
template <typename T>
constexpr T pi = 3.14f;
// Instantiate it.
static_assert(pi<float> == 3.14f);
static_assert(pi<int> == 3);

// Variable template as class member. Must be static.
struct S1 {
    template<typename T>
    static constexpr T val = 1; // Declared inside class, defined inside class.
};
static_assert(S1::val<int> == 1);

template<typename T>
struct S2
{
    static T val; // Declaration of a templated static data member of a class template.
};

template<typename T>
T S2<T>::val = 1; // Definition of the above.

static void demo() {
    assert(S2<int>::val == 1);
}
}


namespace class_with_template_methods {
template <typename T>
struct Struct
{
    T a;
    Struct(T a) : a(a) {}
};

static void demo()
{
    Struct<float> s (1.1f);
    assert(s.a == 1.1f);
}
}



namespace automatic_weight_unit_conversion {
// These are strong types for weights.
// The class automatically converts to the desired unit when it gets passed to a function.
// For example if a function expects a weight in grams,
// and the code passes a weight in kilograms,
// then the compiler automatically converts the passed unit to the expected unit.

// The factors to convert the given weight type to grams.
struct grams
{
    static constexpr float factor2grams = 1.0f;
};

struct kilograms
{
    static constexpr float factor2grams = 1000.0f;
};

// This concept can be used to assure that a type is a weight type.
template <typename T>
concept weight_unit = std::is_same_v<T, grams> or std::is_same_v<T, kilograms>;

template <weight_unit U>
class Weight
{
    float m_value{};

public:
    constexpr Weight() noexcept = default;
    // Constructor taking a float, so the weight is equal to the float passed.
    constexpr explicit Weight(const decltype(m_value) v) noexcept : m_value(v)
    {
    }

    // This copy constructor is a template, the compiler generates multiple constructors:
    // 1. Create a kilogram from a gram.
    // 2. Create a kilogram from a kilogram.
    // 3. Create a gram from a kilogram.
    // 4. Create a gram from a gram.
    template <weight_unit UU>
    constexpr Weight(const Weight<UU>& s) noexcept
    {
        if constexpr (std::is_same_v<U, UU>)
            m_value = s.value();
        else
            m_value = s.value() * UU::factor2grams / U::factor2grams;
    }

    [[nodiscard]] constexpr decltype(m_value) value() const noexcept { return m_value; }
    explicit constexpr operator decltype(m_value)() const noexcept { return m_value; } // Support static cast.
    constexpr void value(const decltype(m_value) v) noexcept { m_value = v; }
    auto operator<=>(const Weight<U>&) const noexcept = default;

    // Allow for basic arithmetic operations:
    constexpr Weight<U>& operator+=(const Weight<U>& s) noexcept
    {
        m_value += s.value();
        return *this;
    }

    constexpr Weight<U>& operator-=(const Weight<U>& s) noexcept
    {
        m_value -= s.value();
        return *this;
    }

    constexpr Weight<U>& operator*=(const Weight<U>& s) noexcept
    {
        m_value *= s.value();
        return *this;
    }

    constexpr Weight<U>& operator*=(const decltype(m_value) v) noexcept
    {
        m_value *= v;
        return *this;
    }

    constexpr Weight<U>& operator/=(const Weight<U>& s) noexcept
    {
        m_value /= s.value();
        return *this;
    }

    constexpr Weight<U>& operator/=(const decltype(m_value) v) noexcept
    {
        m_value /= v;
        return *this;
    }
};

template <weight_unit UL, weight_unit UR>
Weight<UL> operator+(Weight<UL> l, const Weight<UR> r) noexcept { return l += r; }

template <weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator-(Weight<UL> l, const Weight<UR> r) noexcept { return l -= r; }

template <weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator*(Weight<UL> l, const Weight<UR> r) noexcept { return l *= r; }

template <weight_unit U>
constexpr Weight<U> operator*(Weight<U> l, const float r) noexcept { return l *= r; }

template <weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator/(Weight<UL> l, const Weight<UR> r) noexcept { return l /= r; }

template <weight_unit U>
constexpr Weight<U> operator/(Weight<U> l, const float r) noexcept { return l /= r; }

template <weight_unit U>
inline std::ostream& operator<<(std::ostream& os, const Weight<U>& s) noexcept
{
    os << s.value() << " ";
    if constexpr (std::is_same_v<U, grams>)
        os << "g";
    else if constexpr (std::is_same_v<U, kilograms>)
        os << "kg";
    else
        os << "<unknown weight unit>";
    return os;
}

// Some quality tests.
static_assert(std::is_constructible_v<Weight<grams>, float>);
static_assert(std::is_nothrow_constructible_v<Weight<grams>, float>);

static_assert(std::is_default_constructible_v<Weight<grams>>);
static_assert(std::is_nothrow_default_constructible_v<Weight<grams>>);

static_assert(std::is_copy_constructible_v<Weight<grams>>);
static_assert(std::is_trivially_copy_constructible_v<Weight<grams>>);
static_assert(std::is_nothrow_copy_constructible_v<Weight<grams>>);

static_assert(std::is_move_constructible_v<Weight<grams>>);
static_assert(std::is_trivially_move_constructible_v<Weight<grams>>);
static_assert(std::is_nothrow_move_constructible_v<Weight<grams>>);

constexpr auto weight_100_kg = Weight<kilograms>(100);
constexpr auto weight_10_g = Weight<grams>(10);

constexpr Weight<grams> weight_g = weight_100_kg;
static_assert(weight_g.value() == 100000);

constexpr Weight<kilograms> weight_kg = weight_10_g;
static_assert(weight_kg.value() == 0.01f);

}



namespace automatic_weight_units_simple {

struct grams     { static constexpr float factor_to_grams {   1.0f}; };
struct kilograms { static constexpr float factor_to_grams {1000.0f}; };

template <typename U>
concept weight_unit = std::is_same_v<U, grams> or std::is_same_v<U, kilograms>;

template <weight_unit U>
class Weight {
    float m_value{};
public:
    Weight(const float value) : m_value(value) {}

    template <weight_unit UU>
    Weight(const Weight<UU>& s) {
        const float grams = s.value() * UU::factor_to_grams;
        m_value = grams / U::factor_to_grams;
    }

    float value() const { return m_value; }
};

template <weight_unit U>
std::ostream& operator<<(std::ostream& os, const Weight<U>& s) noexcept
{
    os << s.value();
    if constexpr (std::is_same_v<U, grams>)
        os << "g";
    else if constexpr (std::is_same_v<U, kilograms>)
        os << "kg";
    else
        static_assert(false, "not implemented");
    return os;
}

static void demo() {
    const Weight<kilograms> kilogram_1(1);
    const Weight<grams> gram_1000 = kilogram_1;
    {
        std::ostringstream oss;
        oss << gram_1000;
        assert(oss.str() == "1000g");
    }

    const auto fn = [] (const Weight<grams> w) {
        std::ostringstream os;
        os << w;
        return os.str();
    };

    assert(fn(kilogram_1) == "1000g");
    assert(fn(gram_1000) == "1000g");

    assert(fn(Weight<kilograms>{2.2f}) == "2200g");
    assert(fn(Weight<grams>{2.2f}) == "2.2g");
}

}

namespace template_specialization {

// https://en.cppreference.com/cpp/language/template_specialization
// https://en.cppreference.com/cpp/language/partial_specialization


// Generic class template.
template<typename T, typename U>
struct Struct {
    constexpr explicit Struct( T t, U u) : m_t(t), m_u(u) {}
    T m_t;
    U m_u;
};

// Partial class template specialisation.
template<typename T>
struct Struct<T, int> {
    constexpr explicit Struct( T t, const int u) : m_t(t), m_u(u * 2) {}
    T   m_t;
    int m_u;
};

// Full class template specialisation.
template<>
struct Struct<std::string, float> {
    constexpr explicit Struct( std::string t, const float u) : m_t(std::move(t)), m_u(u * 3) {}
    std::string m_t {};
    float       m_u {};
};

// Generic function template.
template<typename T, typename U>
static std::pair<T,U> process(T t, U u)
{
    return {t, u};
}

// Partial function template specialisation.
// template<typename T>
// process<T, int>(T t, int u) { }
// error: function template partial specialization is not allowed

// Solution: use function overloading as alternative.
template<typename T>
static std::pair<T,int> process(T t, int u)
{
    return {++t, ++u};
}

// Full template function specialisation.
template <>
std::pair<std::string, std::string> process<std::string, std::string>(std::string t, std::string u)
{
    return {std::move(t) + "t", std::move(u) + "u"};
}


static void demo()
{
    // Generic class template.
    constexpr Struct<int, unsigned int> storage1(10, 10);
    assert(storage1.m_t == 10 and storage1.m_u == 10);

    // Partially specialized class template.
    const Struct<std::string, int> storage("partial", 50);
    assert(storage.m_t == "partial" and storage.m_u == 100);

    // Fully specialized class template.
    const Struct<std::string, float> storage3("full", 5.0f);
    assert(storage3.m_t == "full" and storage3.m_u == 15.0f);

    // Generic function template.
    const auto result1 = process<unsigned, unsigned>(1u, 2u);
    assert(result1.first == 1u and result1.second == 2u);

    // Using function overloading as partially specialized function template.
    const auto result2 = process<int>(5, 10);
    assert(result2.first == 6 and result2.second == 11);

    // Fully specialised function template.
    const auto result3 = process<std::string, std::string>("1", "2");
    assert(result3.first == "1t" and result3.second == "2u");
}
}


namespace automatic_temperature_unit_conversion {

// Write a template that automatically converts temperatures between different units.
// Kelvin = degrees Celsius + 273.5
// Degrees Celsius = Kelvin - 273.5

struct Kelvin
{
    // Calculators to go from Kelvin to Kelvin and vice versa.
    static constexpr float convert_to_kelvin(const float value) noexcept
    {
        return value;
    }
    static constexpr float convert_from_kelvin(const float value) noexcept
    {
        return value;
    }
};
struct Celsius
{
    // Calculators to go from degrees Celsius to Kelvin and vice versa.
    static constexpr float convert_to_kelvin(const float value) noexcept
    {
        return value + 273.5f;
    }
    static constexpr float convert_from_kelvin(const float value) noexcept
    {
        return value - 273.5f;
    }
};

// Concept to ensure passing a temperature unit only.
template <typename T>
concept temperature_unit = std::is_same_v<T, Kelvin> or std::is_same_v<T, Celsius>;

template <temperature_unit U>
class Temperature
{
    float m_value;
public:

    // Copy constructor from same temperature unit should be OK default.
    constexpr Temperature(const Temperature&) = default;

    // Constructor for a given temperature unit using a float.
    constexpr explicit Temperature(const decltype(m_value) value) noexcept : m_value(value) {};

    // Function to get/set the value
    [[nodiscard]] constexpr decltype(m_value) value() const noexcept { return m_value; };
    constexpr void value(const decltype(m_value) value) noexcept { m_value = value; };

    // Operator to get the value: Supports static cast.
    constexpr explicit operator decltype(m_value) () const noexcept { return m_value; };

    // Automatic comparison operators.
    auto operator <=> (const Temperature&) const noexcept = default;

    // Copy constructor template to create this unit from another unit.
    template <temperature_unit UU>
    constexpr Temperature(const Temperature<UU>& temperature) noexcept
    {
        // Step 1: Convert the incoming temperature to Kelvin.
        const float kelvin = UU::convert_to_kelvin(temperature.value());
        // Step 2: Convert the temperature in Kelvin to the current class's temperature unit.
        m_value = U::convert_from_kelvin(kelvin);
    }
};

constexpr Temperature<Celsius> celsius100 (100);
static_assert(celsius100.value() == 100);
constexpr Temperature<Kelvin> kelvin373 = celsius100;
static_assert(kelvin373.value() == 373.5);

constexpr Temperature<Kelvin> kelvin100(100);
static_assert(kelvin100.value() == 100);
constexpr Temperature<Celsius> celsius173 = kelvin100;
static_assert(celsius173.value() == -173.5);

constexpr Temperature<Celsius> celsius150 (150);
static_assert(celsius150.value() == 150);
constexpr Temperature<Celsius> celsius2 = celsius150;
static_assert(celsius2.value() == 150);
static_assert(celsius150 == celsius2);


static void demo()
{
}
}


namespace automatic_temperature_unit_conversion_simple {
// A class that automatically converts temperatures between different units.
// Kelvin = degrees Celsius + 273.5
// Degrees Celsius = Kelvin - 273.5

struct Celsius;
struct Kelvin;

struct Celsius
{
    float value {};
    operator Kelvin() const;
};

struct Kelvin
{
    float value {};
    operator Celsius() const;
};

Celsius::operator Kelvin() const { return Kelvin (value + 273.5f); }
Kelvin::operator Celsius() const { return Celsius(value - 273.5f); }

float get_kelvin (const Kelvin kelvin)
{
    return kelvin.value;
}

float get_celsius (const Celsius celsius)
{
    return celsius.value;
}

static void demo()
{
    const Kelvin _400kelvin {400};
    assert(get_kelvin(_400kelvin) == 400.0f);
    const Celsius _100celsius {100};
    assert(get_kelvin(_100celsius) == 373.5f);
    assert(get_celsius(_400kelvin) == 126.5f);
}
}

namespace meta_programming_recursive_calculation {
// What is template metaprogramming?
// It means that the compiler, using templates, calculates values at compile time.
// Here is an example of recursion to let the compiler calculate values.

// A factorial of, say, 4 means: multiply all numbers from 4 down to 1.

// This template recursively calls itself to calculate its value.
template <int n>
struct Factorial
{
    static constexpr int value {n * Factorial<n - 1>::value};
};

// This template is specific for a passed value of 0.
template <>
struct Factorial<0>
{
    static constexpr int value {1};
};

// A recursive template is very expensive to process by the compiler.
// A recursive constexpr function is much cheaper.
static constexpr int factorial(const int n) { return n <= 1 ? 1 : n * factorial(n - 1); }

static_assert(Factorial<1>::value == 1);
static_assert(factorial(1) == 1);
static_assert(Factorial<4>::value == 24);
static_assert(factorial(4) == 24);
static_assert(Factorial<6>::value == 720);
static_assert(factorial(6) == 720);


// Another example of recursive template calls.
template <int n>
struct Power
{
    enum { value = 2 * Power<n - 1>::value };
};

template <>
struct Power<0>
{
    enum { value = 1 };
};

static_assert(Power<8>::value == 256);
// The above static assert calculates 2 to the power of 8 (2^8).
// Calculation is done at compile time.
// Essential steps.
// 1. Notice the value passed to Power, in this case value 8.
// 2. The "enum" types could also be constexpr types, they work the same.
// 3. When the compiler sees value 8 to the Power, it instantiates the template.
// 4. The compiler sees that the value from Power<7> is needed.
// 5. The compiler keeps creating new instances of Power<n> till Power<0>.
// 6. The compiler now can create Power<8>.
static void demo()
{
}
}


namespace variadic_function_templates {

template <typename Value, typename... Values>
static constexpr Value sum (Value value, Values... values)
{
    if constexpr (sizeof...(values))
        return value + sum(values...);
    return value;
}

static_assert(sum<int>(1) == 1);
static_assert(sum<float>(1.0f, 2.0f) == 3.0f);
static_assert(sum<unsigned>(1, 2, 3, 4) == 10);

static void demo()
{
}
}


namespace variadic_minimum {
// Calculate minimum of values through variadic template.

// Template for one variable, stops recursion.
template <typename T>
static T min (const T value)
{
    return value;
}

// Template for recursion for more than one variable.
template <typename T, typename ...Args>
static T min (const T value, const Args... args)
{
    const T rest_min = min(args...);
    return value < rest_min ? value : rest_min;
}

static void demo()
{
    assert(min(2.4, 7.5) == 2.4);
    assert(min(2, 3, 4, 5, 6, 1) == 1);
}
}


namespace variadic_class_template {
// A variadic class template can be instantiated with any number of template arguments.
template<typename... Types>
struct Storage
{
    std::tuple<Types...> elements;
    explicit Storage(Types... args) : elements(args...) {}
};

static void demo()
{
    {
        Storage<> s; // Types contains no arguments (this has no application).
    }
    {
        const Storage<int> s(1); // Types contains one argument: int.
        assert(std::get<0>(s.elements) == 1);
    }
    {
        const Storage<int, float> s(1, 1.0f); // Types contains two arguments: int and float.
        assert(std::get<0>(s.elements) == 1);
        assert(std::get<1>(s.elements) == 1.0f);
    }
}
}


namespace pack_expansion {

template <typename ... Args>
static void func1 (Args ... args)
{
}

template <typename ... Args>
static void func2 (Args ... args)
{
    func1(&args...); // &args...   : a pack expansion.
                     // &args      : the pattern.
    // Args ... args expand into: int* i, float* f, char* c, const char** s
    // &args expands into: &i, &f, &c, &s

    func1(1, 1.0f, '1', "1");
    // Args ... args expanded into: int i, float f, char c, const char* s
}


// Pack expansion in function argument lists.
// f(args...);      // Expands into f(a1, a2, a3);
// f(&args...);     // Expands into f(&a1, &a2, &a3);
// f(n, ++args...); // Expands into f(n, ++a1, ++a2, ++a3);
// f(++args..., n); // Expands into f(++a1, ++a2, ++a3, n);

// f(const_cast<const Args*>(&args)...); // Expands into:
// f(const_cast<const A1*>(&a1), const_cast<const A2*>(&a2), const_cast<const A3*>(&a3))

// f(h(args...) + args...); // Expands into:
// f(h(a1, a2, a3) + a1, h(a1, a2, a3) + a2, h(a1, a2, a3) + a3)

// Pack expansion in parentheses works the same as in function argument lists.
// Class c(&args...);     // Expands into Class::Class(&a1, &a2, &a3)
// Class c(n, ++args...); // Expands into Class::Class(n, ++a1, ++a2, ++a3)

// Pack expansion in brace-enclosed initializers.
template <typename... Args>
constexpr int func3 (Args... args) {
    const int size = sizeof...(args) + 2;
    int arr[size] = {1, args..., 2};

    // The initializer lists guarantee sequencing.
    // They can therefore be used to call a function on each element of a pack, in order.
    int sum {0};
    int dummy[sizeof...(Args)] = {(sum += args, 0)...};
    return sum;
}
static_assert (func3(1, 2, 3) == 6);

// Pack expansion in template argument lists.
template <typename T1, typename T2, typename... Args>
static void func4 (T1 t1, T2 t2, Args... args) {
    std::tuple<T1, T2, Args...> tup1; // expands to std::tuple<t1, t2, a1, a2, a3>
    std::tuple<Args..., T1, T2> tup2; // expands to std::tuple<a1, a2, a3, t1, t2>
    std::tuple<T1, Args..., T2> tup3; // expands to std::tuple<t1, a1, a2, a3, t2>
}

// The ellipsis in a function parameter list: the parameter declaration is the pattern for expansion.
template <typename... Ts>
void func5 (Ts ... args) {}
// func5('a', 1); // Ts... expands to void func5(char, int)
// func(0.1f);    // Ts... expands to void func5(float)

template <typename... Ts, int... N>
void func6 (Ts (&...arr)[N]){}

void demo6() {
    int n[1];
    func6<const char, int>("a", n); // Ts (&...arr)[N] expands to:
    //                                        const char (&)[2], int(&)[1]
}

// Pack expansion in base specifiers and member initializer lists.
// template <class...Mixins>
// class C : public Mixins...
// {
// public:
// C (const Mixins&...mixins) : Mixins(mixins...) {}
// };


// Pack expansion in lambda captures.
template<typename... Args>
constexpr int func7(Args... args) {
    auto lambda = [args...] { // <- pack expansion.
        return (args + ...);
    };
    return lambda();
}
static_assert(func7(1,2,3) == 6);


// Pack expansion in the sizeof... operator.
template <typename... Types>
struct Sizeof {
    constexpr static std::size_t size = sizeof... (Types);
};
static_assert(Sizeof<int,char,float>::size == 3);


// Pack expansion in using-declarations.
template <class ... Bases>
struct Derived : Bases...
{
   using Bases::g...;
};
// Derived<B, D> d; // OK: B::g and D::g introduced.


// Pack indexing (C++26).
// consteval auto first_plus_last(auto ... args) {
//     return args...[0] + args...[sizeof...(args) - 1];
// }
// static_assert(first_plus_last(1,2,3) == 4);
// static_assert(first_plus_last(1,2) == 3);
// static_assert(first_plus_last(1) == 2);
// static_assert(first_plus_last(std::string("a")) == "aa");


static void demo()
{
    demo6();
}
}


namespace fold_expressions {
// Replace complex template recursion with elegant one-line syntax for variadic arguments.
// A fold expression applies a binary operator to all elements of a parameter pack,
// reducing them to a single value — without recursion.

// Left and right fold.
// Sample arguments: 1 2 3
// Left: Start from the left, apply operation, get result, then do next argument:
// 1. Do 1 + 2 > result.
// 2. Do result + 3 > final result.
// Right: Start from the right, apply operation, get result, then do next argument:
// 1. Do 2 + 3 > result.
// 2. Do 1 + result > final result.

template <typename ... Args>
static constexpr int sum(Args...args)
{
    return (args + ...);
}
static_assert(sum(1) == 1);
static_assert(sum(2, 3, 4) == 9);


template <typename ... Args>
static constexpr int unary_left_fold(Args&& ... args)
{
    // (((arg1 - arg2) - ...) - argN)
    return (... - args); // Dots at left of operator.
}
// ((1 - 2) - 3)
static_assert(unary_left_fold(1, 2, 3) == -4);

template <typename ... Args>
static constexpr int unary_right_fold(Args&& ... args)
{
    // (arg1 - (... - (argN-1 - argN)))
    return (args - ...); // Dots at right of operator.
}
// (1 - (2 - 3))
static_assert(unary_right_fold(1, 2, 3) == 2);

template <typename I, typename ... Args>
constexpr int binary_left_fold(I init, Args&& ... args)
{
    // ((((init - arg1) - arg2) - ...) - argN)
    return (init - ... - args);
}
// ((10 - 1) - 2) - 3
static_assert(binary_left_fold(10, 1, 2, 3) == 4);

template <typename I, typename ... Args>
constexpr int binary_right_fold(I init, Args&& ... args)
{
    // (arg1 - (... - (argN−1 - (argN - I))))
    return (args - ... - init);
}
// (1 - (2 - (3 - 10)))
static_assert(binary_right_fold(10, 1, 2, 3) == -8);

// Folding over the comma operator.
template <typename... Args>
void comma_operator(std::ostream& os, std::vector<int>& v, Args&&... args)
{
    // Run function on arg1, then on arg2, and so on.
    (void(os << std::forward<Args>(args) << " "), ...);
    (void(v.push_back(args)), ...);
    // Can leave out "void" in both cases.
}

static void demo()
{
    std::ostringstream oss;
    std::vector<int> v;
    comma_operator(oss, v, 1, 2, 3);
    assert(oss.str() == "1 2 3 ");
    assert(v == std::vector<int>({1, 2, 3}));
}
}


namespace template_template_arguments {


template<typename T, template<typename,typename ...> typename C, typename... Args>
std::ostream& operator <<(std::ostream& os, const C<T,Args...>& objs)
{
    os << __PRETTY_FUNCTION__ << '\n';
    for (auto const& obj : objs)
        os << obj << ' ';
    return os;
}

static void demo() {
    return;
    std::vector<float> vf { 1.1, 2.2, 3.3, 4.4 };
    std::cout << vf << '\n';

    std::list<char> lc { 'a', 'b', 'c', 'd' };
    std::cout << lc << '\n';

    std::deque<int> di { 1, 2, 3, 4 };
    std::cout << di << '\n';

}
}

namespace typetrait_specialization_of_vector_v1 {


template <typename V>
concept specialization_of_vector = requires(V& v)
{
    v.begin();
    v.data();
    v.reserve(1);
};

static_assert(    specialization_of_vector<std::vector<int>>);
static_assert(    specialization_of_vector<std::vector<std::string>>);
static_assert(    specialization_of_vector<std::vector<std::map<float,float>>>);
static_assert(not specialization_of_vector<std::list<int>>);
static_assert(not specialization_of_vector<float>);

template <typename V>
requires specialization_of_vector<V>
static void func1 (V& v) { }

static void func2 (specialization_of_vector auto& v) { }

static void demo()
{
    std::vector<int> v;
    func1(v);
    func2(v);
}
}


namespace typetrait_specialization_of_vector_v2 {

template <typename T>
struct is_specialization_of_vector : std::false_type {};

template <typename T>
struct is_specialization_of_vector<std::vector<T>> : std::true_type {};

template <typename T>
concept is_specialization_of_vector_v = is_specialization_of_vector<T>::value;

static_assert(    is_specialization_of_vector_v<std::vector<int>>);
static_assert(    is_specialization_of_vector_v<std::vector<std::string>>);
static_assert(not is_specialization_of_vector_v<std::list<int>>);

const auto func = [](is_specialization_of_vector_v auto& t) {};

static void demo()
{
    std::vector<int> v;
    func(v);
}
}


namespace generic_specialization_of_typetrait {
// See https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf

template <
    typename Arg,
    template<typename... Ts> typename Primary
>
struct is_specialisation_of : std::false_type {};

template <
    typename... Args,
    template<typename... Ts> typename Primary
>
struct is_specialisation_of <Primary<Args...>, Primary> : std::true_type {};

static_assert(    is_specialisation_of<std::vector<int>, std::vector>::value);
static_assert(not is_specialisation_of<std::vector<int>, std::list>::value);
static_assert(    is_specialisation_of<std::vector<std::map<bool,bool>>, std::vector>::value);
static_assert(    is_specialisation_of<std::list<bool>, std::list>::value);
static_assert(    is_specialisation_of<std::map<bool,bool>, std::map>::value);
static_assert(    is_specialisation_of<std::coroutine_handle<void>, std::coroutine_handle>::value);
static_assert(    is_specialisation_of<std::tuple<bool,int,char>, std::tuple>::value);

template <typename T,
          template<typename...Args> typename Primary>
concept is_specialisation_of_v = is_specialisation_of<T, Primary>::value;

template<is_specialisation_of_v<std::vector> T>
static void func(T c) { };


static void demo()
{
    std::vector<int> v = {1,2,3,4};
    func(v);
}
}


namespace is_coroutine_handle {

// Simple concept for whether some type is a coroutine handle.

// A generic handle is std::coroutine_handle<void>
// A task-specific handle could look like std::coroutine_handle<Promise>

// The std::coroutine_handle<void> and std::coroutine_handle<Promise> are treated by the compiler
// as completely different, unrelated types
// Therefore a simple check like std::is_same won't work.
// We need a way to tell the compiler: "Check if this type is a std::coroutine_handle wrapped around anything."

// The solution is a classic C++ metaprogramming technique called "partial template specialization".

// Step 1: The catch-all base case.
// Create a primary template. The default assumption is that the type passed is not a coroutine handle.
// Inherit from std::false_type (which provides a value = false constant).
template <typename T>
struct is_coroutine_handle : std::false_type {};
// When passing int, double, or std::string into this, they don't match any special rules,
// so they hit this base case and return false.

// Step 2: The special filter.
// Write a "special case" (a partial specialization). This tells the compiler:
// "If the type looks exactly like std::coroutine_handle<P>, use this version instead!"
template <typename T>
struct is_coroutine_handle<std::coroutine_handle<T>> : std::true_type {};

// This hits the specialization (true).
static_assert(is_coroutine_handle<std::coroutine_handle<void>>::value);

// This also hits the specialization (true).
struct FakeType {};
static_assert(is_coroutine_handle<std::coroutine_handle<FakeType>>::value);

// This hits the primary template (false).
static_assert(not is_coroutine_handle<int>::value);


static void demo()
{
}
}


namespace explicit_template_instantiation {
// Normally templates are instantiated implicitly by the compiler on first use.
// Explicit template instantiation forces the compiler to instantiate templates before the code uses them.

// Template declarations in templates_explicit.h.

// Template definition in templates_explicit.cpp, linked with the project.
// Explicit definitions in that same file.

static void demo()
{
    // Template usage.
    // It only sees the template function declarations. It does not see the definitions.
    // It relies on the linker to find the definitions.
    assert (func(1, 1) == 2);
}
}


namespace template_deduction_guides {

// The template deduction guide tells the compiler
// how to map constructor arguments to class template parameters.
// It enables class template argument deduction (CTAD).

// Before C++17: Specify types or use helper.
static std::pair<int, float> pair1(1, 2.3f);
static auto pair2 = std::make_pair(1, 2.3f);

// C++17 and later: Types are implicitly deduced.
static std::pair pair3(1, 2.3f);
// Deduced as std::pair<int, float>.
// The compiler writes "implicit deduction guides" to achieve this.

// When the compiler cannot figure out the types,
// then a "user-defined deduction guide" is needed.

// Syntax:
// Map a constructor pattern on the left to the target template type on the right.

template <typename T>
struct Wrapper {
    T value;
    Wrapper(T v) : value(v) {}
};

// Deduction guide forcing decay (converts array types to pointers)
template <typename T>
Wrapper(T) -> Wrapper<std::decay_t<T>>;

static Wrapper w("hello"); // Deducts Wrapper<const char*> instead of Wrapper<const char[6]>

static void demo ()
{
}
}


void demo()
{
    default_type::demo();
    simple_function_template::demo();
    class_template::demo();
    non_type_template_parameter::demo();
    variable_template::demo();
    class_with_template_methods::demo();
    automatic_weight_units_simple::demo();
    template_specialization::demo();
    automatic_temperature_unit_conversion::demo();
    automatic_temperature_unit_conversion_simple::demo();
    meta_programming_recursive_calculation::demo();
    variadic_function_templates::demo();
    variadic_minimum::demo();
    variadic_class_template::demo();
    pack_expansion::demo();
    fold_expressions::demo();
    template_template_arguments::demo();
    typetrait_specialization_of_vector_v1::demo();
    typetrait_specialization_of_vector_v2::demo();
    generic_specialization_of_typetrait::demo();
    is_coroutine_handle::demo();
    explicit_template_instantiation::demo();
    template_deduction_guides::demo();
}
}

