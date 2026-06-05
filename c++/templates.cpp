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

#include "templates.h"

#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <type_traits>
#include <vector>

namespace templates {

// Template programming is about letting the compiler deal with the types.
// Template metaprogramming is about letting the compiler calculate values.

namespace default_type {

template <typename T = int>
constexpr auto sum(T a, T b) -> T { return a + b; }

// Pass the type.
static_assert(std::is_same_v<decltype(sum<unsigned>(1, 2)), unsigned>);
// Omit the type: it takes int.
static_assert(std::is_same_v<decltype(sum<>(1, 2)), int>);
static_assert(std::is_same_v<decltype(sum(1, 2)), int>);

}

namespace simple_function_template {

struct Struct { int v{}; };

// General template.
template <typename T>
T add (T a, T b) noexcept
{
    return a + b;
}

// Specialized template.
template <> // This could be left out.
Struct add (const Struct a, const Struct b) noexcept
{
    return Struct{a.v + b.v };
}

void demo()
{
    add(1,2);
    add(1.0f,2.0f);
    add(std::string("a"), std::string("b"));
    add(Struct(1),Struct(2));
}
}

namespace class_template {

template <typename T>
struct Struct
{
    T a;
    Struct(T a) : a(a) {}
};

void demo()
{
}
}


namespace variable_template {

// Basic variable template.
template <typename T>
constexpr T pi = 3.14f;
// Instantiate it.
static_assert(pi<float> == 3.14f);

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

void demo() {
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
void demo()
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

    // This constructor is a template, the compiler generates multiple constructors:
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

void demo() {
    Weight<kilograms> kilogram_1(1);
    Weight<grams> gram_1000 = kilogram_1;
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

enum Type {none_t, generic_t, float_t} type;

// General class template syntax.
template <typename T>
struct Struct {
    // Generic constructor.
    Struct(T t) {
        type = generic_t;
    }
};

// Specialized class template syntax.
template <>
struct Struct<float> {
    // Specialized constructor for type float.
    Struct(float t) {
        type = float_t;
    }
};

// Key points:
// 1. template<> (no type there)
// 2. Provide class definition for specific type, e.g. Class<float>


// Generic function template.
template <typename T>
void func(T t)
{
    type = generic_t;
}
template <>
void func<float>(float f)
{
    type = float_t;
}

void demo()
{
    assert(type == none_t);
    Struct<int> ci(1);
    assert(type == generic_t);
    Struct<float> cf(1.0f);
    assert(type == float_t);

    func(1);
    assert(type == generic_t);
    func(1.0f);
    assert(type == float_t);
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
    constexpr decltype(m_value) value() const noexcept { return m_value; };
    constexpr void value(decltype(m_value) value) noexcept { m_value = value; };

    // Operator to get the value: Supports static cast.
    constexpr explicit operator decltype(m_value) () const noexcept { return m_value; };

    // Automatic comparison operators.
    auto operator <=> (const Temperature&) const noexcept = default;

    // Copy constructor template to create this unit from another unit.
    template <temperature_unit UU>
    constexpr Temperature(const Temperature<UU>& temperature) noexcept
    {
        // Because of the default copy constructor above, the following is not needed:
        // if constexpr (std::is_same_v<U, UU>)
        //     m_value = temperature.value();

        // Step 1: Convert the incoming temperature to Kelvin.
        // Step 2: Convert the temperature in Kelvin to the current class's temperature unit.
        m_value = U::convert_from_kelvin(UU::convert_to_kelvin(temperature.value()));
    }
};

constexpr Temperature<Celsius> celsius100 (100);
static_assert(celsius100.value() == 100);
constexpr Temperature<Kelvin> kelvin1 = celsius100;
static_assert(kelvin1.value() == 373.5);

constexpr Temperature<Kelvin> kelvin100(100);
static_assert(kelvin100.value() == 100);
constexpr Temperature<Celsius> celsius1 = kelvin100;
static_assert(celsius1.value() == -173.5);

constexpr Temperature<Celsius> celsius150 (150);
static_assert(celsius150.value() == 150);
constexpr Temperature<Celsius> celsius2 = celsius150;
static_assert(celsius2.value() == 150);
static_assert(celsius150 == celsius2);


void demo()
{
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
    static constexpr int value = n * Factorial<n - 1>::value;
};

// This template is specific for a passed value of 0.
template <>
struct Factorial<0>
{
    static constexpr int value = 1;
};

// A recursive template is very expensive to process by the compiler.
// A recursive constexpr function is much cheaper.
constexpr int factorial(int n) { return n <= 1 ? 1 : n * factorial(n - 1); }

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
void demo()
{
}
}


namespace variadic_minimum {
// Calculate minimum of values through variadic template.

// Template for one variable, stops recursion.
template <typename T>
T min (const T value)
{
    return value;
}

// Template for recursion for more than one variable.
template <typename T, typename ...Args>
T min (const T value, const Args... args)
{
    const T rest_min = min(args...);
    return value < rest_min ? value : rest_min;
}

void demo()
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
    Storage(Types... args) : elements(args...) {}
};

void demo()
{
    {
        Storage<> s; // Types contains no arguments (this has no application).
    }
    {
        Storage<int> s(1); // Types contains one argument: int.
        assert(std::get<0>(s.elements) == 1);
    }
    {
        Storage<int, float> s(1, 1.0f); // Types contains two arguments: int and float.
        assert(std::get<0>(s.elements) == 1);
        assert(std::get<1>(s.elements) == 1.0f);
    }
}
}


namespace variadic_function_template {

template <typename V, typename... Values>
constexpr V sum (V value, Values... values)
{
    if constexpr (sizeof...(values))
        return value + sum(values...);
    return value;
}

static_assert(sum<int>(1) == 1);
static_assert(sum<float>(1.0f, 2.0f) == 3.0f);
static_assert(sum<int>(1, 2, 3, 4) == 10);

void demo()
{
}
}


namespace pack_expansion {

template <typename ... Args>
void func1 (Args ... args)
{
}

template <typename ... Args>
void func2 (Args ... args)
{
    func1(&args...); // &args...   : a pack expansion.
                     // &args      : the pattern.
    // Args ... args expand into: int* i, float* f, char* c, const char** s
    // &args expands into: &i, &f, &c, &s

    func2(1, 1.0f, '1', "1");
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
// Class c(&args...);   // Expands into Class::Class(&a1, &a2, &a3)
// Class c(n, ++args...); // Expands into Class::Class(n, ++a1, ++a2, ++a3);

// Pack expansion in brace-enclosed initializers.
template <typename... Args>
constexpr int func3 (Args... args) {
    const int size = sizeof...(args) + 2;
    int arr[size] = {1, args..., 2};

    // The initializer lists guarantee sequencing.
    // They can therefore be used to call a function on each element of a pack, in order.
    int sum = 0;
    int dummy[sizeof...(Args)] = {(sum += args, 0)...};
    return sum;
}
static_assert (func3(1, 2, 3) == 6);

// Pack expansion in template argument lists.
template <typename T1, typename T2, typename... Args>
void func4 (T1 t1, T2 t2, Args... args) {
    std::tuple<T1, T2, Args...> tup1; // expands to std::tuple<t1, t2, a1, a2, a3>
    std::tuple<Args..., T1, T2> tup2; // expands to std::tuple<a1, a2, a3, t1, t2>
    std::tuple<T1, Args..., T2> tup3; // expands to std::tuple<t1, a1, a2, a4, t2>
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


void demo()
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
constexpr int sum(Args...args)
{
    return (args + ...);
}
static_assert(sum(1) == 1);
static_assert(sum(1, 2, 3) == 6);


template <typename ... Args>
constexpr int unary_left_fold(Args&& ... args)
{
    // (((arg1 - arg2) - ...) - argN)
    return (... - args); // Dots at left of operator.
}
// ((1 - 2) - 3)
static_assert(unary_left_fold(1, 2, 3) == -4);

template <typename ... Args>
constexpr int unary_right_fold(Args&& ... args)
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
    (void(os << args << " "), ...);
    ((v.push_back(args)), ...); // Can leave out "void" in both cases.
}

void demo()
{
    std::ostringstream oss;
    std::vector<int> v;
    comma_operator(oss, v, 1, 2, 3);
    assert(oss.str() == "1 2 3 ");
    assert(v == std::vector<int>({1, 2, 3}));
}
}


namespace template_template_arguments {

void demo() {

}
}

namespace typetrait_specialization_of_vector {

template <typename T>
concept specialization_of_vector = requires(T& t)
{
    t.begin();
    t.data();
    t.reserve(1);
};

static_assert(specialization_of_vector<std::vector<int>>);
static_assert(specialization_of_vector<std::vector<std::string>>);
static_assert(specialization_of_vector<std::vector<std::map<float,float>>>);
static_assert(not specialization_of_vector<std::list<int>>);
static_assert(not specialization_of_vector<float>);

template <typename T>
requires specialization_of_vector<T>
void func1 (T& v) { }

void func2 (specialization_of_vector auto& v) { }

void demo()
{
    std::vector<int> v;
    func1(v);
    func2(v);
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

static_assert(is_specialisation_of<std::vector<int>, std::vector>::value);
static_assert(not is_specialisation_of<std::vector<int>, std::list>::value);
static_assert(is_specialisation_of<std::vector<std::map<bool,bool>>, std::vector>::value);
static_assert(is_specialisation_of<std::list<bool>, std::list>::value);
static_assert(is_specialisation_of<std::map<bool,bool>, std::map>::value);

template <typename T,
          template<typename...Args> typename Primary>
concept is_specialisation_of_v = is_specialisation_of<T, Primary>::value;

template<is_specialisation_of_v<std::vector> T>
void func(T c) { };


void demo()
{
    std::vector<int> v = {1,2,3,4};
    func(v);
}
}


void demo()
{
    simple_function_template::demo();
    class_template::demo();
    variable_template::demo();
    class_with_template_methods::demo();
    automatic_weight_units_simple::demo();
    template_specialization::demo();
    automatic_temperature_unit_conversion::demo();
    meta_programming_recursive_calculation::demo();
    variadic_minimum::demo();
    variadic_class_template::demo();
    variadic_function_template::demo();
    pack_expansion::demo();
    fold_expressions::demo();
    template_template_arguments::demo();
    typetrait_specialization_of_vector::demo();
    generic_specialization_of_typetrait::demo();
}
}

