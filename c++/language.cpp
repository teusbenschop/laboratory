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

#include "language.h"

#include <cassert>
#include <string>
#include <vector>


namespace language {


namespace alignment {

static_assert(sizeof(bool) == 1);
static_assert(sizeof(char) == 1);
static_assert(sizeof(int) == 4);
static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);

// Each member of the struct begins at its natural size.
// The entire struct is padded till the size of the largest member.


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

void demo()
{
}
}


namespace alias_declarations_in_init_statements {
void demo()
{
    std::vector v {1, 2, 3};
    for (using T = int; T e : v)
        assert((e));
}
}



namespace literal_suffix_z {
void demo()
{
    // Avoid this warning:
    // comparison of integers of different signs: 'int' and 'size_type' (aka 'unsigned long') [-Wsign-compare]
    const std::vector<int> v{2, 4, 6, 8};
    for (auto i = 0uz; i < v.size(); ++i) {
        assert((v.at(i)));
    }
}
}


namespace extended_floating_point_types {
#ifdef __STDCPP_FLOAT32_T__
std::float32_t f32 = 0.0;
#endif
#ifdef __STDCPP_FLOAT64_T__
std::float64_t f64 = 0.0;
#endif
#ifdef __STDCPP_FLOAT128_T__
std::float128_t f128 = 0.0;
#endif
}



namespace static_operators_and_lambdas {

struct Struct
{
    // Static operators: Can call them without the object instance.
    static int operator()(int a, int b) { return a + b; }
};

void demo()
{
    Struct object;

    // This creates an object (and perhaps the optimizer removes it again).
    assert(Struct{}(1, 0) == 1);

    // This does not create an object. It just calls the static method.
    assert(object(1, 0) == 1);

    // Lambda's can be made static too.
    int x {0};
    const static auto static_lambda = [x] { return x;};
    const auto normal_lambda = [&x] {x+=10; return x;};
    // But see https://godbolt.org/z/3qeqnEsh8 that static lambdas generate much more code.
    // Consider that a static lambda does the capture (of variable x) once.
    // And that may give unexpected output.
    // The rule from R1 of this paper is basically:
    // A static lambda shall have no lambda-capture.
    // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1169r4.html#lambdas
    assert (static_lambda() == 0);
    assert (normal_lambda() == 10);
    assert (static_lambda() == 0);
    assert (normal_lambda() == 20);
}
}


namespace multidimensional_subscript_operator {
void demo()
{
    int array3d[4][3][2]{};
    array3d[3][2][1] = 42;
    assert(array3d[3][2][1] == 42);
}
}



namespace explicit_object_parameter_this {

// Feature in C++23.

// A parameter declaration with the specifier "this" declares an explicit object parameter.

// An explicit object parameter cannot be a function parameter pack,
// and it can only appear as the first parameter of the parameter list
// in the following declarations:
// 1. A declaration of a member function or member function template.
// 2. An explicit instantiation or explicit specialization of a templated member function.
// 3. A lambda declaration.

// A member function with an explicit object parameter has the following restrictions:
// 1. The function is not static.
// 2. The function is not virtual.
// 3. The declarator of the function does not contain cv and ref.

// Error: non-member functions cannot have an explicit object parameter
// void func(this ExplicitObjectParameter& self);

// A pointer to an explicit object member function is an ordinary pointer to function,
// not a pointer to member.

struct Struct
{
    // OK.
    // Same as void foo(int i) const &;
    void f1(this Struct const& self, int i);

    // Error: already declared.
    // void f1(int i) const &;

    // Also OK for templates.
    // For member function templates,
    // explicit object parameter allows deduction of type and value category,
    // this language feature is called “deducing this”.
    template<typename Self>
    void f2(this Self&& self);

    // Pass object by value: makes a copy of “*this”.
    void f3(this Struct self, int i);

    // Error: “const” not allowed here
    // void p(this ExplicitObjectParameter) const;

    // Error: “static” not allowed here
    // static void q(this ExplicitObjectParameter);

    // Error: an explicit object parameter can only be the first parameter
    // void r(int, this ExplicitObjectParameter);

    // Inside the body of an explicit object member function,
    // the "this" pointer cannot be used.
    // All member access must be done through the first parameter,
    // like in static member functions.
    void f4(this Struct object)
    {
        // invalid use of 'this' in a function with an explicit object parameter
        // auto x = this;

        // There's no implicit "this": use of undeclared identifier 'bar'
        // bar();

        object.f3(1);
    }
};

void demo()
{
    struct Y
    {
        int f(int, int) const& {return 1;};
        int g(this Y const& self, int, int) {
            if (&self == &self)
                return 1;
            return 1;
        };
    };

    Y y{};

    const auto pf = &Y::f;

    // error: pointers to member functions are not callable
    // called object type 'int (Y::*)(int, int) const &' is not a function or function pointer
    //pf(y, 1, 2);

    (y.*pf)(1, 2);            // ok
    std::invoke(pf, y, 1, 2); // ok

    auto pg = &Y::g;
    pg(y, 3, 4);              // ok

    // error: “pg” is not a pointer to member function
    // right hand operand to .* has non-pointer-to-member type 'int (*)(const Y &, int, int)'
    //(y.*pg)(3, 4);

    std::invoke(pg, y, 3, 4); // ok
}
}

namespace const_types {

// Immutable:
const int i1 = 10;

// If possible evaluate it at compile time.
constexpr int i2 = 10;

// Will be evaluated at compile time if the parameters "var1" and "var2" are known at compile time.
// Will be evaluated at run time if the parameters are not known at compile time.
constexpr int constexpr_add(int var1, int var2)
{
    return var1 + var2;
}

// Force evaluation of a function at compile time.
consteval int consteval_add(int var1, int var2)
{
    return var1 + var2;
}

// Static initialization. Force evaluation at compile time. The variable is not const.
constinit int i3 = 1;

void demo()
{
    // Can all be evaluated at compile time.
    static_assert(i1 == 10);
    static_assert(i2 == 10);
    static_assert(constexpr_add(1, 2) == 3);
    static_assert(constexpr_add(i1, i2) == 20);

    // Variable i3 is assigned at runtime.
    int i3 = 2;
    // Compile error: the value of 'i3' is not usable in a constant expression
    // static_assert(constexpr_add(i3,i3) == 4);
    assert(i3 == 2);

    // The variable is initialized at runtime although the function is constexpr.
    int i4 = constexpr_add(i3, i3);
    assert(i4 == 4);

    // OK, evaluated at compile-time.
    static_assert(consteval_add(1, 1) == 2);

    // Cannot be evaluated at compile-time.
    // Compile error:
    // call to consteval function 'consteval_add(i3, i3)' is not a constant expression
    // int i5 = consteval_add(i3, i3);

    // Cannot be evaluated at compile-time.
    // Compile error:
    // call to consteval function 'consteval_add(i3, i3)' is not a constant expression
    // int i5 = consteval_add(i3, i3);

    constexpr auto if_constexpr_add = [](auto var1, auto var2)
    {
        // This section is compiled only if it passes, else it's omitted, and so cannot cause compiler errors.
        if constexpr (std::is_same_v<decltype(var1), int>)
            return var1 + var2;
        return var1 + var2;
    };
    static_assert(if_constexpr_add(1, 2) == 3);

    {
        constexpr const auto xdigit = [](int n) -> char
        {
            // This is a constexpr variable in a constexpr lambda function: OK in C++23.
            constexpr const char digits[] = "0123456789";
            return digits[n];
        };
        static_assert(xdigit(2) == '2');
    }
}
}


namespace operator_overloading {
// A struct with overloaded operators.

struct Struct
{
    constexpr Struct(const int value) : value(value)
    {
    }

    int value;

    // Overload the "+" operator.
    constexpr Struct operator+(const Struct& other)
    {
        return Struct(value + other.value);
    }

    // Overload the function call "()" operator, this makes the struct a functor.
    constexpr int operator()() { return value; }

    // Overload the += and the -= and the %= operators.
    constexpr Struct& operator+=(const Struct& c) noexcept
    {
        value += c.value;
        return *this;
    }

    constexpr Struct& operator-=(const Struct& c) noexcept
    {
        value -= c.value;
        return *this;
    }

    constexpr Struct& operator%=(const Struct& c) noexcept
    {
        value = value % c.value;
        return *this;
    }
};

// Overload the "<<" operator.
std::ostream& operator<<(std::ostream& os, const Struct& c) noexcept
{
    os << c.value;
    return os;
}

// Overload the "==" operator.
constexpr inline bool operator==(const Struct& l, const Struct& r) noexcept
{
    return l.value == r.value;
}

static_assert(Struct(10) + Struct(20) == 30);
static_assert(Struct(15)() == 15);
static_assert(Struct(10) != Struct(20));
static_assert(Struct(10) == 10);

void demo()
{
    Struct s(10);
    s += Struct(5);
    assert(s == 15);
    s -= Struct(1);
    assert(s == 14);
    s %= Struct(5);
    assert(s == 4);
}
}



namespace space_ship_operator {
// Demo of the spaceship ( <=> ) operator in C++20.
struct Version
{
    unsigned short major{};
    unsigned short minor{};

    // Setting the spaceship operator to default causes the compiler to generate
    // all comparison operators, like < <= == >= > != .
    // The compiler considers all fields, in this case major and minor.
    // If the first field is smaller than or greater than, the comparison is complete.
    // If the first fields are the same,
    // then it considers the second field, and so on,
    // till it completes the comparison.
    constexpr auto operator<=>(const Version&) const noexcept = default;
};

static_assert(Version(1, 1) != Version(1, 2));
static_assert(Version(1, 1) < Version(1, 2));
static_assert(Version(1, 1) <= Version(1, 2));
static_assert(Version(1, 2) > Version(1, 1));
static_assert(Version(1, 2) >= Version(1, 1));
static_assert(Version(1, 1) == Version(1, 1));

constexpr double foo{-0.0f};
constexpr double bar{+0.0f};
constexpr std::partial_ordering result{foo <=> bar};
static_assert(result != std::partial_ordering::less);
static_assert(result != std::partial_ordering::greater);
static_assert(result == std::partial_ordering::equivalent);
static_assert(result != std::partial_ordering::unordered);

void demo(){}
}


void demo() {
    alignment::demo();
    alias_declarations_in_init_statements::demo();
    literal_suffix_z::demo();
    static_operators_and_lambdas::demo();
    multidimensional_subscript_operator::demo();
    explicit_object_parameter_this::demo();
    const_types::demo();
    operator_overloading::demo();
    space_ship_operator::demo();
}

}
