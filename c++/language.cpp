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
#include <cmath>
#include <functional>
#include <source_location>
#include <string>
#include <type_traits>
#include <vector>

#include "clocking.h"


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


namespace at_exit {
void demo()
{
    const auto fn = []() -> void
    {
        assert(true); // To see the effect on exit: Set to false.
    };
    std::atexit(fn); // Register "fn" to run at normal program exit.
}
}


namespace attribute_assume {
// Specifies that the given expression is assumed to always evaluate to true
// to allow compiler optimizations based on the information given.
// Since assumptions cause runtime-undefined behavior if they do not hold,
// they should be used sparingly.
// https://en.cppreference.com/w/cpp/language/attributes/assume
// One correct way to use them is to follow assertions with assumptions.

[[maybe_unused]] auto f = [] (auto x) {
    // Compiler may assume x is positive.
    assert(x > 0);
    [[assume(x > 0)]];
};

void demo() {}

}


namespace attribute_likely_unlikely {
// https://en.cppreference.com/w/cpp/language/attributes/likely
// Attribute to hint the compiler for the likely or unlikely path of execution,
// allowing the compiler to optimize the code.
constexpr double power(double x, long long n) noexcept
{
    if (n > 0) [[likely]]
      return x * std::pow(x, n - 1);
    else [[unlikely]]
      return 1;
}

constexpr long long factorial(long long n) noexcept
{
    if (n > 1) [[likely]]
      return n * factorial(n - 1);
    else [[unlikely]]
      return 1;
}

constexpr double cosine(double x) noexcept
{
    constexpr long long precision{16LL};
    double y{};
    for (auto n{0LL}; n < precision; n += 2LL) [[likely]]
      y += pow(x, n) / (n & 2LL ? -factorial(n) : factorial(n));
    return y;
}

void demo()
{

}
}



namespace attribute_no_unique_address {
// https://en.cppreference.com/w/cpp/language/attributes/no_unique_address

struct Empty {}; // The size of any object or empty class type is at least 1.
static_assert(sizeof(Empty) == 1);

struct X
{
    int i; // 4 bytes.
    Empty e; // At least one more byte is needed to give ‘e’ a unique address
};
static_assert(sizeof(X) == 8);

struct Y
{
    int i; // 4 bytes.
    [[no_unique_address]] Empty e; // Empty member optimized out.
};
static_assert(sizeof(Y) == 4);

struct Z
{
    char c;
    // e1 and e2 cannot share the same address because they have the
    // same type, even though they are marked with [[no_unique_address]].
    // However, either may share address with ‘c’.
    [[no_unique_address]] Empty e1, e2;
};
static_assert(sizeof(Z) == 2);

struct W
{
    char c[2];
    // e1 and e2 cannot have the same address, but one of
    // them can share with c[0] and the other with c[1]:
    [[no_unique_address]] Empty e1, e2;
};
static_assert(sizeof(W) == 3);

void demo()
{
}
}


namespace basic_memory_management {
void demo()
{
    struct Struct
    {
        Struct(const std::string& name) : name(name)
        {
        }
        std::string name;
    };

    // Established way.
    {
        // Allocate sufficient memory for the object.
        auto* memory = std::malloc(sizeof(Struct));
        // Construct new object in existing memory.
        auto* object = new(memory) Struct("hello");
        assert(object->name == "hello");
        // Call destructor: This does not yet free the memory.
        object->~Struct();
        // Free memory on heap.
        std::free(memory);
    }

    // Modern way in C++20.
    {
        auto* memory = std::malloc(sizeof(Struct));
        auto* struct_ptr = static_cast<Struct*>(memory);
        // Method 1:
        std::uninitialized_fill_n(struct_ptr, 1, Struct{"hello"});
        assert(struct_ptr->name == "hello");
        // Method 2:
        std::construct_at(struct_ptr, Struct{"hello"});
        assert(struct_ptr->name == "hello");
        // And destroy / free again.
        std::destroy_at(struct_ptr);
        std::free(memory);
    }
}
}


namespace remove_const_volatile_reference {
// Demo of removing const, volatile, and reference.
static_assert(std::is_same_v<std::remove_cvref_t<int>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<int&>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<int&&>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<const int&>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<const int[2]>, int[2]>);
static_assert(std::is_same_v<std::remove_cvref_t<const int(&)[2]>, int[2]>);
static_assert(std::is_same_v<std::remove_cvref_t<int(int)>, int(int)>);

void demo()
{
    auto power = [](const auto& v, int n)
    {
        //auto product = decltype(v){1};
        //typename std::remove_cvref<decltype(v)>::type product {1};
        std::remove_cvref_t<decltype(v)> product{1};
        for (int i = 0; i < n; i++)
        {
            product *= v;
        }
        return product;
    };
    assert(power(2, 4) == 16);
}
}


namespace simple_type_traits {
static_assert(std::is_same_v<uint8_t, unsigned char>);
static_assert(std::is_floating_point_v<decltype(3.f)>);
static_assert(std::is_unsigned_v<unsigned int>);
static_assert(not std::is_unsigned_v<int>);

class Planet
{
};

class Mars : public Planet
{
};

class Sun
{
};

static_assert(std::is_base_of_v<Planet, Mars>);
static_assert(std::derived_from<Mars, Planet>);
static_assert(std::is_convertible_v<Mars, Planet>);
static_assert(not std::is_base_of_v<Planet, Sun>);
static_assert(not std::is_base_of_v<Mars, Planet>);

static_assert(std::is_scoped_enum_v<int> == false);

class A
{
};

static_assert(not std::is_scoped_enum_v<A>);

enum B { self_test = std::is_scoped_enum_v<B> };

static_assert(not std::is_scoped_enum_v<B>);
static_assert(not self_test);

enum struct C
{
};

static_assert(std::is_scoped_enum_v<C>);

enum class D : int
{
};

static_assert(std::is_scoped_enum_v<D>);

enum class E;
static_assert(std::is_scoped_enum_v<E>);

// The following types are collectively called implicit-lifetime types:
// * scalar types:
//     * arithmetic types
//     * enumeration types
//     * pointer types
//     * pointer-to-member types
//     * std::nullptr_t
// * implicit-lifetime class types
//     * is an aggregate whose destructor is not user-provided
//     * has at least one trivial eligible constructor and a trivial,
//       non-deleted destructor
// * array types
// * cv-qualified versions of these types.
// static_assert(std::is_implicit_lifetime_v<int>); // arithmetic type is a scalar type
// static_assert(std::is_implicit_lifetime_v<const int>); // cv-qualified a scalar type

// enum E { e };
//    static_assert(std::is_implicit_lifetime_v<E>); // enumeration type is a scalar type
//    static_assert(std::is_implicit_lifetime_v<int*>); // pointer type is a scalar type
//    static_assert(std::is_implicit_lifetime_v<std::nullptr_t>); // scalar type

// struct S
// {
//     int x, y;
// };
//    S is an implicit-lifetime class: an aggregate without user-provided destructor
//    static_assert(std::is_implicit_lifetime_v<S>);
//
//    static_assert(std::is_implicit_lifetime_v<int S::*>); // pointer-to-member

// struct X { ~X() = delete; };
// X is not implicit-lifetime class due to deleted destructor
//    static_assert(!std::is_implicit_lifetime_v<X>);
//
//    static_assert(std::is_implicit_lifetime_v<int[8]>); // array type
//    static_assert(std::is_implicit_lifetime_v<volatile int[8]>); // cv-qualified array type
//
//    static_assert(std::reference_constructs_from_temporary_v<int&&, int> == true);
//    static_assert(std::reference_constructs_from_temporary_v<const int&, int> == true);
//    static_assert(std::reference_constructs_from_temporary_v<int&&, int&&> == false);
//    static_assert(std::reference_constructs_from_temporary_v<const int&, int&&> == false);
//    static_assert(std::reference_constructs_from_temporary_v<int&&, long&&> == true);
//    static_assert(std::reference_constructs_from_temporary_v<int&&, long> == true);


void demo()
{
    struct foo
    {
        void m()
        {
        }

        void m() const
        {
        }

        void m() volatile
        {
        }

        void m() const volatile
        {
        }
    };
    foo{}.m();
    std::add_const_t<foo>{}.m();
    std::add_volatile_t<foo>{}.m();
    std::add_cv_t<foo>{}.m(); // Add const volatile.
}
}


namespace source_location {
// https://en.cppreference.com/w/cpp/utility/source_location
void demo()
{
    constexpr int line = __LINE__;
    const std::source_location location = std::source_location::current();
    const std::string file_name = location.file_name();
    assert(file_name.contains("language.cpp"));
    assert(location.line() == line + 1);
    assert(location.column() == 43);
    assert(location.function_name() == std::string("void language::source_location::demo()"));
}
}


namespace references {
// General rule of thumb about whether something is a lvalue or rvalue reference.
// 1. If you can take the address of an expression, the expression is a lvalue.
// 2. If the expression is a lvalue reference (e.g., T& or const T&, etc.),
// that expression is a lvalue.
// Otherwise, the expression is a rvalue.
// Conceptually, rvalues correspond to temporary objects,
// such as those returned from functions or created through implicit type conversions.
// Most literal values (e.g., 10 and 5.3) are rvalues.

namespace lvalue_references {
// The lvalue reference aliases an existing object (optionally with different cv-qualification).
void demo()
{
    // When a function's return type is lvalue reference, the function call expression becomes a lvalue expression.
    auto char_number = [] (std::string& s, std::size_t n) -> char&
    {
        return s.at(n); // The string::at() returns a reference to char.
    };
    std::string str = "ab";
    char_number(str, 1) = 'a'; // The function call is lvalue, can be assigned to.
    assert(str == "aa");
}
}


namespace rvalue_references {
void demo()
{
    // This function returns an rvalue of temporary pair {10,10}.
    // Normally this pair {10,10} goes out of scope at function end.
    auto pair10 = [] -> std::pair<int,int> { return {10,10}; };

    // Rvalue binds to temporary object, extends lifetime of it.
    std::pair<int,int>&& r1 = pair10();
    int&& r2 = r1.first + r1.first;
    assert(r2 == 20);

    // Can modify the rvalue through reference to non-const.
    r1.first++;
    assert(r1.first == 11);
}
}

namespace reference_collapsing {
// Have references to references.
// A rvalue reference to rvalue reference collapses to rvalue reference.
// All other combinations form lvalue reference.
void demo()
{
    typedef int&  lref;
    typedef int&& rref;
    int n;

    lref&  r1 = n;
    static_assert(std::is_lvalue_reference_v<decltype(r1)>);

    lref&& r2 = n;
    static_assert(std::is_lvalue_reference_v<decltype(r2)>);

    rref&  r3 = n;
    static_assert(std::is_lvalue_reference_v<decltype(r3)>);

    rref&& r4 = 1;
    static_assert(std::is_rvalue_reference_v<decltype(r4)>);
}
}



namespace forwarding_references {
// T&& does not always mean: rvalue reference.

// General rule of thumb:
// If a variable or parameter is declared to have type T&& for some deduced type T,
// that variable or parameter is a forwarding reference (or: universal reference).
// A 'const' makes it a rvalue reference: const T&&

// The forwarding (universal) reference makes perfect forwarding possible.

template<typename T>
T func(T&& t) // t is forwarding reference
{
    return std::forward<T>(t); // Perfect forwarding.
}

void demo()
{
    int i = 1;
    int& i2 = func(i); // Returns lvalue reference.
    int&& i3 = func(1); // Returns rvalue reference.
    int&& i4 = func(std::move(i)); // Returns rvalue reference.

    auto&& i5 = func(i); // i5 is forwarding reference, initialized by lvalue.
    auto&& i6 = func(std::move(i)); // i6 is forwarding reference, initialized by rvalue.
}
}


namespace dangling_references {
// If the referred-to object goes out of scope before the reference,
// the reference becomes dangling.

// const std::string& f()
// {
//     std::string s = "Test";
//     return s; // exits the scope of s:
//     // its destructor is called and its storage deallocated
// }

void demo()
{
//     const std::string& r = f(); // dangling reference
//     //assert(r == "Test");; // undefined behavior: reads from a dangling reference
//     //std::string s = f(); // undefined behavior: copy-initializes from a dangling reference
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



void demo()
{
    lvalue_references::demo();
    rvalue_references::demo();
    reference_collapsing::demo();
    forwarding_references::demo();
    dangling_references::demo();
    reference_wrappers::demo();
}
}


namespace final {

struct base {
    virtual int f() const = 0;
};

struct derived final : base {
    int f() const override { return 1; }
};

void demo()
{
    // According to the theory the function call in a final class is faster
    // due to devirtualization, so the compiler knows that derived.f can only call the overriding f.
    // But time measurements could not detect a real improvement.
    int sum = 0;
    {
        //scoped_timer::scoped_timer<std::chrono::nanoseconds> timer;
        for (int i = 0; i < 1000; i++)
        {
            derived derived;
            sum += derived.f();
        }
    }
    assert(sum == 1000);
}
}


namespace enums {

// An unscoped enum.
enum Enum1 {red, green, yellow};
constexpr Enum1 e = green;
static_assert(e == green);

// Unscoped enum with assignment.
enum Enum2 {a2, b2, c2 = 10, d2 = c2 + 5};
static_assert(a2 == 0);
static_assert(b2 == 1);
static_assert(c2 == 10);
static_assert(d2 == 15);

// Scoped enum.
enum class ScopedEnum1 { red, green = 10, yellow };
ScopedEnum1 se1 = ScopedEnum1::red;

// Demo using enum.
enum class Fruit { apple, pear };
struct S
{
    using enum Fruit; // This brings apple and pear into the struct scope.
};
S s;
static_assert(s.apple == Fruit::apple);

// Demo class enum with specified underlying type.
enum class Height : char { low = 'l', high = 'h' };
static_assert(sizeof(Height::low) == 1);

void demo()
{
}
}


namespace noexcept_specifier {
// noexcept : the function is declared not to throw.

// Results in std::terminate no matter whether caller uses try...catch block.
void noexcept_but_throw () noexcept
{
    // throw 1;
}

void f() noexcept; // the function f() does not throw
void (*fp)() noexcept(false); // fp points to a function that may throw
void g(void pfa() noexcept);  // g takes a pointer to function that doesn't throw
typedef int (*pf)() noexcept; // should be error but compiles on macOS.

// No function overload when differing in exception specification.
void f() noexcept;
//void f(); // error: different exception specification
void g() noexcept(false);
void g(); // ok, both declarations for g are potentially-throwing

// Overriding class methods must have the same exception specification, unless deleted.
struct A
{
    virtual void f() noexcept;
    virtual void g();
    virtual void h() noexcept = delete;
};
struct B : A
{
    //void f(); // Error: Base function is potentially-throwing, derived is not
    virtual void g() noexcept; // OK.
    virtual void h() noexcept = delete; // OK: deleted.
};

// Template function with noexcept true/false depending on type size.
template <typename T>
void func1() noexcept(sizeof(T) < 4);

decltype(func1<char>()) func2();

void demo()
{
}
}


namespace keyword_const {

struct S
{
    // Functions with different const qualifiers have different types so may overload each other.
    int& get() { return i; }
    const int& get () const { return i; }
    int i {10};
};


void demo()
{
    S s1;
    s1.get()++; // Calls the first.
    int i1 = s1.get(); // Calls the first.

    const S s2;
    int i2 = s2.get(); // Calls the second.
}
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
    at_exit::demo();
    attribute_assume::demo();
    attribute_likely_unlikely::demo();
    attribute_no_unique_address::demo();
    basic_memory_management::demo();
    remove_const_volatile_reference::demo();
    simple_type_traits::demo();
    source_location::demo();
    references::demo();
    final::demo();
    enums::demo();
    noexcept_specifier::demo();
    keyword_const::demo();
}

}
