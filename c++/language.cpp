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

#include <cassert>
#include <charconv>
#include <cmath>
#include <expected>
#include <functional>
#include <new>
#include <ranges>
#include <source_location>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <__ranges/transform_view.h>
#include "language.h"
#include "clocking.h"


namespace language {

namespace alignment {

static_assert(sizeof(bool) == 1);
static_assert(sizeof(char) == 1);
static_assert(sizeof(int) == 4);
static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);

// Each member of the struct begins at its natural size.
// The entire struct is padded till a multiple of the size of the largest member.


struct S1
{
    bool b; // 1 byte
    // padding 3 bytes
    int i; // 4 bytes
};
static_assert(alignof(S1) == 4);
static_assert(sizeof(S1) == 8);

struct S2
{
    double d; // 8 bytes
    bool b; // 1 byte
    // padding 7 bytes
};
static_assert(alignof(S2) == 8);
static_assert(sizeof(S2) == 16);

struct S3
{
    int i1; // 4 bytes
    // padding 4 bytes
    double d; // 8 bytes
    int i2; // 4 bytes
    // padding 4 bytes
};
static_assert(alignof(S3) == 8);
static_assert(sizeof(S3) == 24);

struct S4
{
    int i1; // 4 bytes
    int i2; // 4 bytes
    double d; // 8 bytes
};
static_assert(alignof(S4) == 8);
static_assert(sizeof(S4) == 16);

struct S5
{
    double d; // 8 bytes
    bool b1; // 1 byte
    // 3 bytes padding
    int i; // 4 bytes
    bool b2; // 1 byte
    // 7 bytes padding
};
static_assert(alignof(S5) == 8);
static_assert(sizeof(S5) == 24);

struct S6
{
    bool b1;
    bool b2;
};
static_assert(sizeof(S6) == 2);
static_assert(alignof(S6) == 1);

// Can specify desired alignment.
namespace {
struct alignas(16) S7 { };
}
static_assert(alignof(S7) == 16);
static_assert(sizeof(S7) == 16);


static void demo()
{
}
}


namespace alias_declarations_in_init_statements {
static void demo()
{
    for (using I = int; const I e : {1, 2})
        assert((e));
}
}



namespace literal_suffix_z {
static void demo()
{
    // Avoid this warning:
    // comparison of integers of different signs: 'int' and 'std::size_type' (aka 'unsigned long') [-Wsign-compare]
    constexpr std::array c{2, 4, 6, 8};
    for (auto i = 0uz; i < c.size(); ++i) {
        assert(c.at(i));
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

namespace {
struct Struct
{
    // Static operators: Can call them without the object instance.
    static int operator()() { return 2; }
};
}

static void demo()
{
    // This creates an object (and the optimizer may remove it again).
    assert(Struct{}() == 2);

    // This calls the static method on the already created object.
    constexpr Struct s;
    assert(s() == 2);

    // This does not create an object. It just calls the static method.
    assert(Struct::operator()() == 2);

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
static void demo()
{
    int array3d[4][3][2]{};
    assert(array3d[3][2][1] == 0);
    array3d[3][2][1] = 321;
    assert(array3d[3][2][1] == 321);
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
    // void f1(int i) const&;

    // Also OK for templates.
    // For member function templates,
    // explicit object parameter allows deduction of type and value category,
    // this language feature is called “deducing this”.
    template<typename Self>
    void f2(this Self&& self) {};

    // Pass object by value: makes a copy of “*this”.
    void f3(this Struct self, int i) {};

    // Error: “const” not allowed here
    // void p(this Struct) const;

    // Error: “static” not allowed here
    // static void q(this Struct);

    // Error: an explicit object parameter can only be the first parameter
    // void r(int, this Struct);

    // Inside the body of an explicit object member function,
    // the "this" pointer cannot be used.
    // All member access must be done through the first parameter,
    // like in static member functions.
    void f4(this Struct object)
    {
        // Invalid use of 'this' in a function with an explicit object parameter
        // auto x = this;

        // There's no implicit "this": use of undeclared identifier 'bar'
        // bar();

        object.f3(1);
    }
};

static void demo()
{
    struct Y
    {
        int f(int, int) const& {return 1;};
        int g(this Y const& self, int, int) {
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

    const auto pg = &Y::g;
    pg(y, 3, 4);              // ok

    // error: “pg” is not a pointer to member function
    // right hand operand to .* has non-pointer-to-member type 'int (*)(const Y &, int, int)'
    // (y.*pg)(3, 4);

    std::invoke(pg, y, 3, 4); // ok
}
}


namespace const_types_and_positions {

// Force evaluation of a function at compile time.
// (Does not work with variables.)
consteval int consteval_add(int v1, int v2)
{
    return v1 + v2;
}

// Static initialization. Force evaluation at compile time. The variable is not const.
static constinit int i3 = 1;

static void demo()
{
    // Immutable: can be evaluated at compile time.
    const int i1 = 10;
    static_assert(i1 == 10);

    // If possible evaluate it at compile time: Can be evaluated at compile time.
    constexpr int i2 = 10;
    static_assert(i2 == 10);

    // Will be evaluated at compile time if the parameters "var1" and "var2" are known at compile time.
    // Will be evaluated at run time if the parameters are not known at compile time.
    constexpr auto constexpr_add = [](const int var1, const int var2)
    {
        return var1 + var2;
    };
    static_assert(constexpr_add(1, 2) == 3);
    static_assert(constexpr_add(i1, i2) == 20);

    // The global is repeated here for clarity.
    // constinit int i3 = 1;
    // Variable i3 is assignable at runtime.
    i3 = 2;
    assert(i3 == 2);
    // Compile error: the value of 'i3' is not usable in a constant expression
    // static_assert(constexpr_add(i3,i3) == 4);

    // The variable is initialized at runtime although the function is constexpr.
    const int i4 = constexpr_add(i3, i3);
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

    constexpr auto if_constexpr_add = [](auto v1, auto v2)
    {
        // This section is compiled only if it passes, else it's omitted, and so cannot cause compiler errors.
        if constexpr (std::is_same_v<decltype(v1), int>)
            return v1 + v2;
        return v1 + v2;
    };
    static_assert(if_constexpr_add(1, 2) == 3);

    {
        constexpr auto digit_n = [](const int n) -> char
        {
            // This is a constexpr variable in a constexpr lambda function: OK in C++23.
            constexpr char digits[] = "0123456789";
            return digits[n];
        };
        static_assert(digit_n(2) == '2');
    }

    // Const at left of asterisk: What is pointed to is immutable. Pointer is mutable.
    const char* left_const {"data is immutable"};
    left_const++;
    // *left_const = 'a'; fails to compile.

    // Const at right of asterisk: The pointer is immutable. Data is mutable.
    char a = 'a';
    char* const right_const = std::addressof(a);
    *right_const = 'b';
    // right_const++; fails to compile.
    assert(a == 'b');

    // Iterator itself is const. Data is mutable.
    {
        std::vector v {1, 2};
        const decltype(v)::iterator iter = v.begin();
        *iter = 2;
        // iter++; // Fails to compile because iter is const.
    }

    // Iterator is not const. Data is const.
    {
        std::vector<int> v {1, 2};
        decltype(v)::const_iterator iter = v.cbegin();
        assert(*iter == 1);
        ++iter;
        assert(*iter == 2);
        // *iter = 3; // Fails to compile because data is const.
    }
}
}


namespace operator_overloading {

struct Struct
{
    explicit constexpr Struct(const int value) : value(value) { }

    int value;

    // Define the "+" operator.
    constexpr Struct operator+ (const Struct& other) const noexcept
    {
        return Struct(value + other.value);
    }

    // Define the call operator, this makes the struct a functor (a function object).
    constexpr decltype(value) operator()() const noexcept
    {
        return value;
    }

    // Define the += operator (similar -= , %=, and so on.
    constexpr Struct& operator+= (const Struct& other) noexcept {
        value += other.value;
        return *this;
    }

    // Implicit type conversion operator.
    // Enables static_cast.
    explicit constexpr operator float() const noexcept
    {
        return static_cast<float>(value);
    }
};

// Overload the "<<" operator.
static std::ostream& operator<< (std::ostream& os, const Struct& s) noexcept
{
    os << s.value;
    return os;
}

// Overload the "==" operator.
static constexpr bool operator==(const Struct& lhs, const Struct& rhs) noexcept
{
    return lhs.value == rhs.value;
}


static_assert(Struct(10) + Struct(20) == Struct(30));
static_assert(Struct(15)() == 15);
static_assert(Struct(10)() == 10);
static_assert(Struct(10) != Struct(20));
static_assert(Struct(10) == Struct(10));

// Never overload common operators like && || , as that only confuses others.

static void demo()
{
    Struct s(10);
    s += Struct(5);
    assert(s == Struct(15));
    // Same as above, but one line:
    assert((Struct{10} += Struct(5)) == Struct(15));
}
}



namespace spaceship_operator {
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
    // then it considers the second field in the same way, and so on,
    // till it completes the comparison.
    constexpr auto operator<=>(const Version&) const noexcept = default;
};

static_assert(Version(1, 1) != Version(1, 2));
static_assert(Version(1, 1) <  Version(1, 2));
static_assert(Version(1, 1) <= Version(1, 2));
static_assert(Version(2, 1) >  Version(1, 2));
static_assert(Version(1, 2) >= Version(1, 1));
static_assert(Version(1, 1) == Version(1, 1));

constexpr double foo{-0.0f};
constexpr double bar{+0.0f};
constexpr std::partial_ordering result{foo <=> bar};
static_assert(result != std::partial_ordering::less);
static_assert(result != std::partial_ordering::greater);
static_assert(result == std::partial_ordering::equivalent);
static_assert(result != std::partial_ordering::unordered);

static void demo(){}
}


namespace at_exit {
static void demo()
{
    const auto exit_fn = []
    {
        assert(true); // To see the effect on exit: Set to false.
    };
    std::atexit(exit_fn); // Register "fn" to run at normal program exit.
}
}


namespace attribute_assume {
// Specifies that the given expression is assumed to always evaluate to true
// to allow compiler optimizations based on the information given.
// Since assumptions cause runtime-undefined behavior if they do not hold,
// they should be used sparingly.
// https://en.cppreference.com/w/cpp/language/attributes/assume
// A correct way to use them is to precede them with assertions.

static auto func = [] (auto x) {
    // Compiler may assume x is positive.
    assert(x > 0);
    [[assume(x > 0)]];
};

static void demo()
{
    func(1);
}

}


namespace attribute_likely_unlikely {
// https://en.cppreference.com/w/cpp/language/attributes/likely
// Attribute to hint the compiler for the likely or unlikely path of execution,
// allowing the compiler to optimize the code.
static constexpr double power(const double x, const long long n) noexcept
{
    if (n > 0) [[likely]]
      return x * std::pow(x, n - 1);
    [[unlikely]]
    return 1;
}

static constexpr long factorial(const long n) noexcept
{
    if (n > 1) [[likely]]
      return n * factorial(n - 1);
    [[unlikely]]
    return 1;
}

static void demo()
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

static void demo()
{
}
}


namespace basic_memory_management {
// Modern memory management in C++20.
static void demo()
{
    struct Struct
    {
        explicit Struct(std::string name) : name(std::move(name))
        {
        }
        std::string name;
    };

    // Allocate sufficient memory for the object.
    auto* memory = std::malloc(sizeof(Struct));
    auto* struct_ptr = static_cast<Struct*>(memory);
    // Construct new object in existing memory.
    // Method 1:
    std::uninitialized_fill_n(struct_ptr, 1, Struct{"hello"});
    assert(struct_ptr->name == "hello");
    // Construct new object in existing memory.
    // Method 2:
    std::construct_at(struct_ptr, Struct{"hello"});
    assert(struct_ptr->name == "hello");
    // Call destructor: This does not yet free the memory.
    std::destroy_at(struct_ptr);
    // Free memory on heap.
    std::free(memory);
}
}


namespace remove_const_volatile_reference {
// Demo of removing const / volatile / reference.
static_assert(std::is_same_v<std::remove_cvref_t<int>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<int&>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<int&&>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<const int&>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<const int[2]>, int[2]>);
static_assert(std::is_same_v<std::remove_cvref_t<const int(&)[2]>, int[2]>);
static_assert(std::is_same_v<std::remove_cvref_t<int(int)>, int(int)>);

static void demo()
{
    auto power = []<typename Value>(const Value& value, const int n) noexcept
    {
        std::remove_cvref_t<Value> product{1};
        for (int i = 0; i < n; i++)
            product *= value;
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

class Earth : public Planet
{
};

class Sun
{
};

static_assert(std::is_base_of_v<Planet, Earth>);
static_assert(std::derived_from<Earth, Planet>);
static_assert(std::is_convertible_v<Earth, Planet>);
static_assert(not std::is_base_of_v<Planet, Sun>);
static_assert(not std::is_base_of_v<Earth, Planet>);

static_assert(not std::is_scoped_enum_v<int>);

class A { };
static_assert(not std::is_scoped_enum_v<A>);

enum B { self_test = std::is_scoped_enum_v<B> };

static_assert(not std::is_scoped_enum_v<B>);
static_assert(not self_test);

enum struct C { };
static_assert(std::is_scoped_enum_v<C>);

enum class D : long { };
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
//     * has at least one trivial eligible constructor
//       and a trivial non-deleted destructor
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

// https://en.cppreference.com/w/cpp/types/common_reference.html
// Determines the common reference type of the types T...,
// that is, the type to which all the types in T... can be converted or bound.
static_assert(std::same_as<int&, std::common_reference_t<
              std::add_lvalue_reference_t<int>,
              std::add_lvalue_reference_t<int>&,
              std::add_lvalue_reference_t<int>&&
              >>);

static void demo()
{
    struct foo
    {
        void m() { }
        void m() const { }
        void m() volatile { }
        void m() const volatile { }
    };
    foo{}.m();
    std::add_const_t<foo>{}.m();
    std::add_volatile_t<foo>{}.m();
    std::add_cv_t<foo>{}.m(); // Add const volatile.
}
}


namespace source_location {
// https://en.cppreference.com/w/cpp/utility/source_location
static void demo()
{
    constexpr int line = __LINE__;
    constexpr std::source_location location = std::source_location::current();
    const std::string file_name = location.file_name();
    assert(file_name.contains("language.cpp"));
    static_assert(location.line() == line + 1);
    assert(location.column() == 47);
    static_assert(location.function_name() == std::string_view("void language::source_location::demo()"));
}
}


namespace value_categories {
// The glvalue (generalized lvalue).
// 1. Has address in memory.
// 2. Can it be moved or hijacked?
//    * No:  lvalue.
//    * Yes: xvalue (expiring value).

// The rvalue.
// * Can it be moved or hijacked?
//   * No:  prvalue (pure rvalue).
//   * Yes: xvalue (expiring value).

//            Expression
//           /         \
//       glvalue     rvalue
//      /      \     /     \
//   lvalue    xvalue    prvalue

// Conceptually, rvalues correspond to temporary objects,
// such as those returned from functions or created through implicit type conversions.
// Most literal values (e.g., 10 and 5.3) are rvalues.

static void demo()
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"

    // The lvalue: has address, cannot be moved from.
    int x{};
    x;
    std::cin;
    "hello"; // string literals have address.
    ++x; // result of prefix increment.
    int& get_val(); // has address of int.
    int&& r = 42; // named var -> lvalue (regardless of its type).

    // The prvalue: no address, can be moved from.
    42;
    x + 1;
    std::string("hello"); // temporary.
    x++; // result of postfix increment.
    true;

    // The xvalue: has address, can be moved from.
    std::move(x);
    static_cast<int&&>(x);
    std::string("hello")[0];
    int&& some_func();

#pragma clang diagnostic pop
}
}


namespace value_category_references {

namespace lvalue_references {
// The lvalue reference aliases an existing object (optionally with different cv-qualification).
static void demo()
{
    {
        // When a function's return type is an lvalue reference,
        // the function call expression becomes an lvalue expression.
        auto char_number = [] (std::string& s, const std::size_t n) -> char&
        {
            return s.at(n); // The string::at() returns a reference to char.
        };
        std::string str = "ab";
        char_number(str, 1) = 'a'; // The function call is lvalue, can be assigned to.
        assert(str == "aa");
    }

    {
        // A reference is like an alias.
        // Once it refers a variable, it can never be updated to refer to another variable.
        int one = 1;
        constexpr int two = 2;
        int& int_ref = one;
        // Assignment to reference does not change what the reference points to but changes the referenced object.
        int_ref = two;
        assert(int_ref == two);
        assert(one == two);
    }
}
}


namespace rvalue_references {
static void demo()
{
    // This function returns a rvalue of temporary pair {10,10}.
    // Normally this pair {10,10} goes out of scope at function end.
    auto pair10 = [] -> std::pair<int,int> { return {10,10}; };

    // Rvalue reference binds to temporary object, extends its lifetime.
    std::pair<int,int>&& r1 = pair10();
    int&& r2 = r1.first + r1.second;
    assert(r2 == 20);

    // Can modify the rvalue through reference to non-const.
    ++r1.first;
    assert(r1.first == 11);
}
}


namespace rvalue_reference_function_parameter {
static void f1(float&&) { }

// Pass a rvalue reference to the function.
static void f2 (float&& v)
{
    // The variable v is now a lvalue within the function scope (because it has a name).
    // f1(v); error: no matching function for call to f1
    f1(std::move(v)); // Must change to rvalue, then call function.
}

static void demo()
{
    f2(1.0f);
}
}


namespace reference_collapsing {
// Have references to references.
// An rvalue reference to rvalue reference collapses to rvalue reference.
// All other combinations collapse to an lvalue reference.
static void demo()
{
    using l_val_ref = int&;
    using r_val_ref = int&&;

    int n = 0;

    l_val_ref&  r1 = n;
    static_assert(std::is_lvalue_reference_v<decltype(r1)>);

    l_val_ref&& r2 = n;
    static_assert(std::is_lvalue_reference_v<decltype(r2)>);

    r_val_ref&  r3 = n;
    static_assert(std::is_lvalue_reference_v<decltype(r3)>);

    r_val_ref&& r4 = 1;
    static_assert(std::is_rvalue_reference_v<decltype(r4)>);
}
}


namespace forwarding_references {
// T&& does not always mean: rvalue reference.

// General rule of thumb:
// If a variable or parameter is declared to have type T&& for some deduced type T,
// that variable or parameter is a forwarding reference (or universal reference).
// A 'const' makes it a rvalue reference: const T&&

// The forwarding (universal) reference makes perfect forwarding possible.

template<typename T>
T func(T&& t) // t is forwarding reference
{
    return std::forward<T>(t); // Perfect forwarding.
}

static void demo()
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

const std::string& f()
{
    std::string s = "s";
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-stack-address"
    return s; // exits the scope of s:
    // Its destructor is called and its storage deallocated.
#pragma GCC diagnostic pop
}

static void demo()
{
    const std::string& r = f(); // dangling reference
    // assert(r == "s");; // undefined behavior: reads from a dangling reference
    // std::string s = f(); // undefined behavior: copy-initializes from a dangling reference
}
}


namespace reference_wrappers {
static void demo()
{
    const auto fn = [](int& n1, int& n2, const int& n3)
    {
        n1++; // Increases the copy of n1 stored in the function object.
        n2++; // Increases the caller's n2.
        // n3++; Compile error: cannot assign to variable 'n3' with const-qualified type 'const int &'
    };

    int n1 = 1;
    int n2 = 3;
    const int n3 = 5;

    const std::function<void()> bound_fn = std::bind(fn, n1, std::ref(n2), std::cref(n3));

    bound_fn();

    assert(n1 == 1); // This is left unchanged, because it was passed by value to the bind call.
    assert(n2 == 4); // This was passed by reference, and got increased by the function.
    assert(n3 == 5); // Passed by const reference, could not get increased.
}
}


namespace decltype_with_double_brackets {
// Single brackets decltype(x) gives the declared type of the variable x.
// Double brackets decltype((x)) treat x as an expression.
static void demo()
{
    // lvalue -> lvalue reference.
    int x{};
    static_assert(std::is_lvalue_reference_v<decltype((x))>);

    // prvalue -> plain type = T
    static_assert(std::is_same_v<decltype((10)), int>);

    // xvalue -> rvalue reference = T&&
    static_assert(std::is_rvalue_reference_v<decltype((std::move(x)))>);
}
}


static void demo()
{
    lvalue_references::demo();
    rvalue_references::demo();
    rvalue_reference_function_parameter::demo();
    reference_collapsing::demo();
    forwarding_references::demo();
    dangling_references::demo();
    reference_wrappers::demo();
    decltype_with_double_brackets::demo();
}
}


namespace embed {
// C++26 feature.
static void demo()
{
    constexpr unsigned char d[]
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc23-extensions"
#embed "embed.txt"
#pragma clang diagnostic pop
    };
    std::ranges::for_each(d, [](auto c)
    {
//        std::cout << c << std::endl;
    });
}
}

namespace final {

struct base {
    virtual ~base() = default;
    virtual int f() const = 0;
};

struct derived final : base {
    int f() const override { return 1; }
};

static void demo()
{
    // According to the theory the function call in a final class is faster
    // due to devirtualization, so the compiler knows that derived.f can only call the overriding f.
    // But time measurements could not detect an improvement.
    int sum = 0;
    {
        // scoped_timer::scoped_timer<std::chrono::nanoseconds> timer;
        for (int i = 0; i < 1000; ++i)
        {
            derived derived;
            sum += derived.f();
        }
    }
    assert(sum == 1000);
}
}


namespace enums {
namespace {

// An unscoped enum.
enum Enum1 {red, green, yellow};
constexpr Enum1 e1 = green;
static_assert(e1 == green);

// Unscoped enum with assignment.
enum Enum2 {a2, b2, c2 = 10, d2 = c2 + 5};
static_assert(a2 == 0);
static_assert(b2 == 1);
static_assert(c2 == 10);
static_assert(d2 == 15);

// Scoped enum.
enum class ScopedEnum1 { red, green = 10, yellow };
constexpr auto se1 = ScopedEnum1::red;

// Demo using enum.
enum class Fruit { apple, pear };
struct S
{
    using enum Fruit; // This brings apple and pear into the struct scope.
};
constexpr S s;
static_assert(s.apple == Fruit::apple);

// Default underlying type is int.
constexpr auto f1 = Fruit{0};
constexpr Fruit f2 {10}; // Compiles, but 10 is outside enum.

// Demo class enum with specified underlying type.
enum class Height : char { low = 'l', high = 'h' };
static_assert(sizeof(Height::low) == 1);

// Iterating over enums: Define prefix increment operator.
enum class Iterable {_start_, one, two, three, _end_};
Iterable& operator++(Iterable& iter)
{
    auto i = std::to_underlying(iter);
    return iter = static_cast<std::decay_t<decltype(iter)>>(++i);
}

}
static void demo()
{
    int sum{0};
    for (auto iter = Iterable::_start_; iter <= Iterable::_end_; ++iter)
    {
        sum += std::to_underlying(iter);
    }
    assert(sum == 10); // 0 + 1 + 2 + 3 + 4.
}
}


namespace noexcept_specifier {
// noexcept : the function is declared not to throw.

// Results in std::terminate no matter whether caller uses try...catch block.
static void noexcept_but_throw () noexcept
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

// Overriding class methods must have the same exception specification or stricter, unless deleted.
struct A
{
    virtual void f() noexcept;
    virtual void g();
    virtual void h() noexcept = delete;
};
struct B : A
{
    //void f(); // error: exception specification of overriding function is more lax than base version
    void g() noexcept override; // OK.
    void h() noexcept override = delete; // OK: deleted.
};

// Template function with noexcept true/false depending on type size.
template <typename T>
void func1() noexcept(sizeof(T) < 4);

void func2() noexcept(true);
static_assert(std::is_same_v<decltype(func1<char>), decltype(func2)>);

static void demo()
{
}
}


namespace keyword_const {

struct Struct
{
    // Functions with different const qualifiers have different types so may overload one another.
    int& get() { return i1; }
    const int& get() const { return i2; }
    int i1 {10};
    int i2 {20};
};

static void demo()
{
    Struct s1;
    int i1 = s1.get(); // Calls the first.
    assert(i1 == 10);
    i1 = ++s1.get(); // Calls the first.
    assert(i1 == 11);

    constexpr Struct s2;
    const int i2 = s2.get(); // Calls the second.
    assert(i2 == 20);
}
}


namespace unspecified_order_of_evaluation {
// Order of evaluation of any part of any expression is unspecified.
// Not to confuse this with the associativity of operators.

static int a() { return std::puts("a"); }
static int b() { return std::puts("b"); }
static int c() { return std::puts("c"); }
static void z(int, int, int) {}

static void demo()
{
    // z(a(), b(), c()); // all 6 permutations of output are allowed, e.g. "b c a".
    // int i = a() + b() + c(); // all 6 permutations of output are allowed, e.g. "c b a".
}
}


namespace inline_specifier {
inline int i = 0; // Inline variable.
inline void demo() { } // Inline function.
}


namespace structured_binding {
static void demo() {
    {
        // Binding to an array.
        constexpr int a[2] = {1, 2};
        // Use copies. The c0 is copied from a[0] and c1 from a[1].
        auto [c0, c1] = a;
        // Use references. The r0 refers to a[0] and r1 to a[1].
        auto& [r0, r1] = a;
    }
    {
        // Binding to a tuple.
        float f1 {};
        char  c1 {};
        int   i1 {};
        std::tuple<float&, char&&, int> tpl (f1, std::move(c1), i1);
        const auto& [f2, c2, i2] = tpl;
        // Variable f2 refers to f1 (initialized from get<0>(tpl)).
        static_assert(std::is_same_v<decltype(f2), float&>);
        // Variable c2 refers to c1 (initialized from get<1>(tpl)).
        static_assert(std::is_same_v<decltype(c2), char&&>);
        // Variable i2 refers to the third component of tpl, get<2>(tpl).
        static_assert(std::is_same_v<decltype(i2), const int>);
    }
    {
        // Binding to data members.
        struct S
        {
            mutable int i;
            char c;
        };
        auto func = [] -> S { return S{1, '1'}; };
        const auto [i2, c2] = func();
        assert(i2 == 1);
        assert(c2 == '1');
        i2 = 2;       // OK because member is mutable.
        // c2 = '2';  // Error: c2 is const-qualified
    }
}
}


namespace copy_elision {
// Copy elision does not create a temporal copy of an object.
// It creates the object directly into the target.
// It also omits side effects of the used constructor or destructor.
// Programs that rely on these side effects are not portable.
// Copy elision is used for return statements or throw expressions.
static void demo()
{
}
}


namespace casting {
static void demo()
{
    // The static_cast converts one type to a related type.
    // Converts between types using a combination of implicit and user-defined conversions.
    {
        const int i = 10;
        uint ui = static_cast<uint>(i);
    }

    // The dynamic_cast converts within inheritance hierarchies.
    // Safely converts pointers and references to classes up, down, and sideways along the inheritance hierarchy.
    // Will have impact on performance.
    {
        struct Base
        {
            virtual ~Base() = default;
        };
        struct Derived : Base { };
        Base base1;
        Derived* derived1 = dynamic_cast<Derived*>(&base1);

        Derived derived2;
        Base& base2 = dynamic_cast<Base&>(derived2);

        try
        {
            struct Unrelated {};
            auto& unrelated1 = dynamic_cast<Unrelated&>(base1);
            assert(false); // Never gets here.
        }
        catch (const std::bad_cast& exception)
        {
            assert(exception.what() == std::string("std::bad_cast"));
        }
    }

    // The const_cast adds or removes cv-qualifiers.
    // Converts between types with different cv-qualification.
    {
        constexpr int i1 {0};
        int& i2 = const_cast<int&>(i1);
        ++i2;
    }

    // The reinterpret_cast converts type to unrelated type.
    // Converts between types by reinterpreting the underlying bit pattern.
    {
        int8_t rc1 = 1;
        auto* rc2 = reinterpret_cast<uint8_t*>(&rc1);
    }

    // General rules:
    // 1. Avoid cast if possible.
    // 2. Avoid C-style casts.
}
}




namespace custom_new_and_delete {
// Declare operator new in global namespace, see bottom.

// Class-specific new/delete operators: static function.
struct S
{
    static void* operator new(const std::size_t count)
    {
        std::cout << "class new for size " << count << std::endl;
        return ::operator new(count);
    }
    static void operator delete(void* ptr)
    {
        std::cout << "class delete for pointer " << ptr << std::endl;
        ::operator delete(ptr);
    }
};


static void demo()
{
    //std::unique_ptr<S> s = std::make_unique<S>(S());
}
}


namespace prefix_and_postfix_increment_operators {
static void demo()
{
    int i{0};
    ++i; // More efficient: No copying.
    i++; // Returns copy of old value: Less efficient.
    ++++i; // Increase twice.
    // i++++; operator++ returns const: not assignable.

    {
        struct S
        {
            int val;
            // Signature of prefix increment operator.
            S& operator++() { ++val; return *this; }
            // Signature of postfix increment operator - takes dummy int which compiler fills with 0.
            S operator++(int) { const int copy = val; ++val; return S{copy}; }
            constexpr auto operator <=> (const S& other) const noexcept = default;
        };
        S s {1};
        assert(++s   == S{2});
        assert(  s++ == S{2}); // Increase object, return object with previous value.
        assert(  s   == S{3});
    }
}
}


namespace unnamed_namespace {
// Everything in an unnamed namespace is local to the translation unit.
// In some ways equal to the "static" keyword.
namespace {
int i = 0;
}
static void demo() { };
}


namespace argument_dependent_lookup {
static void demo()
{
    // Argument-Dependent Lookup (ADL),
    // also known as Koenig Lookup,
    // is a core mechanism in C++
    // that allows the compiler to find a function
    // without an explicit namespace prefix
    // by searching the namespace(s) of the function's argument(s).
}
}


namespace conditional_operator {
// The expression form of the "if" statement.

static_assert(0 == 0 ? true : false); // Simple form.

constexpr auto three_way_compare = [](const int lhs, const int rhs) consteval
{
    return lhs == rhs ? 0 : lhs < rhs ? -1 : +1; // Chaining two operators.
};
static_assert(three_way_compare (1, 1) ==  0);
static_assert(three_way_compare (2, 3) == -1);
static_assert(three_way_compare (4, 3) == +1);

static void demo()
{
}
}


namespace to_underlying_and_underlying_type_t {
// https://en.cppreference.com/w/cpp/utility/to_underlying.html
// Converts an enumeration to its underlying type.
// Equivalent to: return static_cast<std::underlying_type_t<Enum>>(e);.
static void demo()
{
    enum class EnumClass : unsigned { e };
    static_assert(std::is_same_v<unsigned, decltype(std::to_underlying(EnumClass::e))>);
    static_assert(std::is_same_v<unsigned, std::underlying_type_t<EnumClass>>);

    enum class ColorMask : std::uint32_t
    {
        red   =  0xFF,
        green = (red   << 8),
        blue  = (green << 8),
        alpha = (blue  << 8)
    };
    static_assert(std::is_same_v<std::uint32_t, decltype(std::to_underlying(ColorMask::red))>);
    static_assert(std::is_same_v<std::uint32_t, std::underlying_type_t<ColorMask>>);
}
}


namespace unreachable_and_fallthrough {
// https://en.cppreference.com/w/cpp/utility/unreachable.html
// Invokes undefined behavior at a given point.
static void demo()
{
    enum class Values {a, b, c} ec;

    const auto func = [](const Values input)
    {
        switch (input)
        {
        using enum Values;
        case a: [[fallthrough]];
        case b: [[fallthrough]];
        case c: return "c";
        default:
            std::unreachable(); // Invokes undefined behaviour, perhaps optimize this away.
        }
    };

    func(Values::c);
}
}


namespace monadic_operations_on_optional {
// Monadic operations (transform, or_else, and and_then)
// for std::optional (P0798R8) and std::expected (P2505R5)
static void demo()
{
    // and_then
    // Returns the result of the given function on the contained value if it exists,
    // or an empty optional otherwise.

    // transform
    // Returns an optional containing the transformed contained value if it exists,
    // or an empty optional otherwise.

    // or_else
    // Returns the optional itself if it contains a value,
    // or the result of the given function otherwise.

    const std::vector<std::optional<std::string>> v
    {
        "1234", "15 foo", "bar", "42", "5000000000", " 5", std::nullopt, "-43"
    };

    const auto transform_func = [](auto&& o) {

        const auto to_int = [](std::string_view sv) -> std::optional<int>
        {
            int r{};
            auto [ptr, ec]{std::from_chars(sv.data(), sv.data() + sv.size(), r)};
            if (ec == std::errc())
                return r;
            else
                return std::nullopt;
        };

        using namespace std::literals;

        return o
        // if optional is nullopt convert it to optional with "" string
          .or_else([]{ return std::optional{""s}; })
        // flatmap from strings to ints (making empty optionals where it fails)
          .and_then(to_int)
        // map int to int + 1
          .transform([](int n) { return n + 1; })
        // convert back to strings
          .transform([](int n) { return std::to_string(n); })
        // replace all empty optionals that were left by
        // and_then and ignored by transforms with "NaN"
          .value_or("NaN");
    };

    const auto output = v | std::ranges::views::transform(transform_func) | std::ranges::to<std::vector<std::string>>();

    const std::vector<std::string> standard  {
        "1235", "16", "NaN", "43", "NaN", "NaN", "NaN", "-42"
    };
    assert (output == standard);

}
}


namespace monadic_operations_on_expected {
static void demo()
{
    const std::vector<std::optional<std::string>> inputs{
        "1234",
        "15 foo",
        "bar",
        "42",
        "5000000000",
        " 5",
        std::nullopt,
        "-43"
    };

    // Instance std::expected<T, E> provides a way to store either of two values.
    // An instance of std::expected always holds a value:
    // either the expected value of type T, or the unexpected value of type E.
    // This vocabulary type requires the header <expected>.
    // With std::expected you can implement functions that either return a value or an error.

    const auto get_int = [](const auto input) -> std::expected<int, std::string> {
        try {
            return std::stoi (input.value_or(""));
        } catch (const std::exception& exception) {
            return std::unexpected(exception.what());
        }
    };

    // Template std::expected supports monadic operations for convenient function composition.
    // expected.and_then
    // Returns the result of the given function call if it exists or an empty std::expected.
    // expected.transform
    // Returns a std::expected containing its transformed value or an empty std::expected.
    // expected.or_else
    // Returns the std::expected if it contains a value.
    // Otherwise returns the result of the given function.
    // expected.transform_error
    // Returns the std::expected if it contains an expected value.
    // Otherwise returns a std::expected that contains a transformed unexpected value.

    for (const auto& input : inputs) {
        // std::cout << std::left << std::setw(12) << std::quoted(input.value_or("nullopt")) << " -> ";
        const auto result = get_int(input)
          .transform( [](const int n) { return n + 1; })
        ;
        if (result)
        {
            // std::cout << result.value();
        }
        else
        {
            // std::cout << result.error();
        }
        // std::cout << std::endl;
    }
}
}


namespace ref_qualified_member_functions {

namespace {
class Task {
public:
    explicit Task(std::string name) : m_name(std::move(name)) {}

    // This overload is selected when called on an lvalue.
    // The object lives on, so don't steal from it.
    std::string execute() const & {
        return "lvalue overload " + m_name;
    }

    // This overload is selected when called on an rvalue.
    // The object is about to die, so it might be safe to move its resources out.
    std::string execute() && {
        const std::string stolen = std::move(m_name);  // Steal the data.
        return "rvalue overload " + stolen;
    }

private:
    std::string m_name;
};
}

static Task make_task() { return Task("rvalue"); }

static void demo()
{
    Task t("lvalue");
    assert (t.execute() == "lvalue overload lvalue");
    assert (make_task().execute() == "rvalue overload rvalue");
    assert (std::move(t).execute() == "rvalue overload lvalue");
}
}


void demo() {
    alignment::demo();
    alias_declarations_in_init_statements::demo();
    literal_suffix_z::demo();
    static_operators_and_lambdas::demo();
    multidimensional_subscript_operator::demo();
    explicit_object_parameter_this::demo();
    const_types_and_positions::demo();
    operator_overloading::demo();
    spaceship_operator::demo();
    at_exit::demo();
    attribute_assume::demo();
    attribute_likely_unlikely::demo();
    attribute_no_unique_address::demo();
    basic_memory_management::demo();
    remove_const_volatile_reference::demo();
    simple_type_traits::demo();
    source_location::demo();
    value_categories::demo();
    value_category_references::demo();
    embed::demo();
    final::demo();
    enums::demo();
    noexcept_specifier::demo();
    keyword_const::demo();
    unspecified_order_of_evaluation::demo();
    inline_specifier::demo();
    structured_binding::demo();
    copy_elision::demo();
    casting::demo();
    custom_new_and_delete::demo();
    prefix_and_postfix_increment_operators::demo();
    unnamed_namespace::demo();
    argument_dependent_lookup::demo();
    conditional_operator::demo();
    to_underlying_and_underlying_type_t::demo();
    unreachable_and_fallthrough::demo();
    monadic_operations_on_optional::demo();
    monadic_operations_on_expected::demo();
    ref_qualified_member_functions::demo();
}

}
