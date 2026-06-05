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

#include <algorithm>
#include <any>
#include <cassert>
#include <complex>
#include <functional>
#include <iostream>
#include <list>
#include <random>
#include <thread>
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
static_assert(p1.first.construction == from_tuple);
static_assert(p1.second.construction == from_tuple);

// This creates the same as above but then piecewise, so from an int and a float.
constexpr std::pair<Foo, Foo> p2{std::piecewise_construct, tuple, tuple};
static_assert(p2.first.construction == from_int_float);
static_assert(p2.second.construction == from_int_float);


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
    auto rvalue = [] -> int
    {
        return 100;
    };

    // The variable i is a lvalue.
    int lvalue = 100;

    [[maybe_unused]] auto tuple = std::forward_as_tuple(lvalue, rvalue());

    // A lvalue reference binds to a lvalue. Marked with one ampersand (&).
    // A rvalue reference binds to a rvalue. Marked with two ampersands (&&).
    static_assert(std::is_same_v<decltype(tuple), std::tuple<int&, int&&>>);
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


namespace initialization {
void demo()
{
    std::string s1; // Default initialization.
    //std::string s2(); // Not an initialization but a function declaration.
    std::string s3 = "s3"; // Copy initialization.
    std::string s4("s4"); // Direct initialization.
    std::string s5{'a'}; // List initialization.
    char c1[3] = {'1', '2', '3'}; // Aggregation initialization.
    char& c2 = c1[0]; // Reference initialization.
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




namespace enable_shared_from_this {
// The std::enable_shared_from_this allows an object
// that is currently managed by a std::shared_ptr
// to safely generate additional std::shared_ptr instances, pt1, pt2 etc.,
// that all share ownership of the object with the original shared_ptr.
void demo()
{
    struct Struct : std::enable_shared_from_this<Struct>
    {
        std::shared_ptr<Struct> get_ptr()
        {
            return shared_from_this();
        }
    };

    // The original and derived shared pointers share the same object.
    {
        std::shared_ptr<Struct> object1 = std::make_shared<Struct>();
        std::shared_ptr<Struct> object2 = object1->get_ptr();
        assert(object1 == object2);
        assert(object1.use_count() == 2);
        assert(object2.use_count() == 2);
    }

    // Bad use: shared_from_this is called without having std::shared_ptr owning the caller.
    // It will throw an exception.
    try
    {
        Struct object1;
        std::shared_ptr<Struct> object2 = object1.get_ptr();
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
    Struct<int> s = {1, 2, 3, 4, 5}; // Copy list-initialization.
    s.append({6, 7, 8}); // List-initialization in function call.
    assert(s.c_arr().second == 8); // Assert the backing array size.
    assert(s.vec.size() == 8);

    // Range-for over brace-init-list.
    for (int x : {-1, -2, -3}) // The rule for auto makes this ranged-for work.
        assert(x == -3 or x == -2 or x == -1);

    auto il1 = {10, 11, 12}; // Special rule for auto.
    assert(il1.size() == 3);

    auto il_copy = il1; // A shallow-copy of top-level proxy object.
    assert(il_copy.begin() == il1.begin()); // Guaranteed: backing array is the same.

    std::initializer_list<int> il2{-3, -2, -1};
    assert(il2.begin()[2] == -1); // Note the replacement for absent operator[] .
    il2 = il1; // Shallow-copy.
    assert(il2.begin() == il1.begin()); // Guaranteed.

    // templated_fn({1, 2, 3}); // Compiler error! "{1, 2, 3}" is not an expression.
    // It has no type, and so T cannot be deduced.
    templated_fn<std::initializer_list<int>>({1, 2, 3}); // OK
    templated_fn<std::vector<int>>({1, 2, 3}); // Also OK
}
}


namespace pseudo_random_number_generation {
void demo()
{
    {
        // Poisson distribution.
        // The probability of an event happening a certain number of times within a given interval of time.
        // Construct it around a given mean event count.
        constexpr float mean_event_count{4.5};
        std::poisson_distribution<int> distribution(mean_event_count);
        std::default_random_engine generator;

        // Store how often an event occurs.
        constexpr int array_size{10};
        std::array<int, array_size> events{};

        constexpr int number_of_experiments{1000};
        [[maybe_unused]] constexpr int number_of_stars{100};

        for (int i = 0; i < number_of_experiments; ++i)
        {
            const int number = distribution(generator);
            if (number < array_size) events[number]++;
        }

        // The Poisson distribution is around a given mean event count.
        for (int i = 0; i < array_size; ++i)
        {
            //std::cout << i << ": " << std::string(events[i] * number_of_stars / number_of_experiments, '*') << std::endl;
        }
        // 0: *
        // 1: *****
        // 2: ***********
        // 3: ******************
        // 4: ******************
        // 5: *****************
        // 6: ***********
        // 7: ********
        // 8: ****
        // 9: **

        // Simulate throwing a 6-sided dice.
        {
            // A seed source for the random number engine.
            std::random_device rd;
            // A Mersenne Twister_random engine seeded with the random device.
            std::mt19937 mt_generator(rd());
            // A uniform distribution between 1 and 6 inclusive.
            std::uniform_int_distribution<> uniform_distribution(1, 6);

            // Throw the dice multiple times.
            // std::cout << "Normal distribution for throwing a dice" << std::endl;
            // for (int n = 0; n < 20; n++)
            //     std::cout << uniform_distribution(mt_generator) << " ";
            // std::cout << std::endl;
            // The above will have a normal distribution like this:
            // 4 5 6 6 3 6 6 1 6 1 6 1 1 4 2 2 1 5 1 2
        }
    }
}
}


namespace heterogenous_collections_with_variant {
void demo()
{
    using variant_t = std::variant<int, std::string, bool>;
    {
        auto container = std::vector<variant_t> { false, "hello", "world", 13 };
        std::ranges::reverse(container);
    }
    {
        constexpr std::string needle {"needle"};
        const auto v = std::vector<variant_t>{42, needle, true};
        for (const auto& item : v) {
            std::visit([](auto&& x)
            {
            //assert ((x == 42) or (std::string(x) == needle) or (x == true));
            // std::cout << "Variant has: " << x << std::endl;
            }, item);
        }
        const auto num_bools = std::ranges::count_if(v, [](const auto& item) {
          return std::holds_alternative<bool>(item);
        });
        assert(num_bools == 1);
        auto contains_needle_string = std::ranges::any_of(v, [&](const auto& item) {
          return std::holds_alternative<std::string>(item) and std::get<std::string>(item) == needle;
        });
        assert(contains_needle_string);
    }
}
}


namespace range_fillup {
void demo() {
    {
        // Demonstration of ranges fill.
        std::vector<int> v(5);
        std::ranges::fill(v, 123);
        // The output will be:  123 123 123 123 123
        const std::vector<int> standard = {123, 123, 123, 123, 123};
        assert(v == standard);
    }
    {
        // Demonstration of ranges generate.
        std::vector<int> v(4);
        std::ranges::generate(v, std::rand);
        // The output will be random, e.g. 16807 282475249 1622650073 984943658
    }
    {
        // Demonstration of ranges iota.
        auto v = std::list<int>(6);
        // Fill the list with ascending values: 0, 1, 2, ...
        std::iota(v.begin(), v.end(), 0);
        const auto standard = std::list<int>({0, 1, 2, 3, 4, 5});
        assert(v == standard);
        //std::ranges::iota(v, 0);

        // A vector of iterators.
        // Fill with iterators to consecutive list's elements.
        std::vector<std::list<int>::iterator> vec(v.size());
        std::iota(vec.begin(), vec.end(), v.begin());
        //std::ranges::iota(vec.begin(), vec.end(), list.begin());
    }
}
}


namespace aliases {

// Type alias ( = typedef).
using vi = std::vector<int>;
static_assert(std::is_same_v<vi, std::vector<int>>);

// Alias template.
template <typename T>
using Pair = std::pair<T, T>;
Pair<int> p {1, 2}; // Equals std::pair<int, int>.
static_assert(std::is_same_v<Pair<int>, std::pair<int, int>>);

void demo()
{
}
}


namespace perfect_forwarding {
// Perfect forwarding means that the function forwards its arguments
// without changing its lvalue or rvalue.

std::string value;
void overloaded_function(int&  i) { value = "lvalue"; }
void overloaded_function(int&& i) { value = "rvalue"; }

template<typename Arg>
void template_function(Arg&& arg)
{
    overloaded_function(std::forward<Arg>(arg));
}

void demo()
{
    int i = 10;
    template_function(i);
    assert (value == "lvalue");

    template_function(10);
    assert (value == "rvalue");
}
}


namespace storage_duration {

// "static" variables last for the duration of the program.

// "thread_local" variables last for the duration of the thread.
// The storage for these entities lasts for the duration of the thread in which they are created.
// There is a distinct object or reference per thread.
// Use of the declared name refers to the entity associated with the current thread.
thread_local int thread_local_var = 0;

void demo()
{
    // Instance 1 of the variable.
    thread_local_var++;
    {
        std::jthread thread ([]()
        {
            // Instance 2 of the variable.
            thread_local_var++;
            assert(thread_local_var == 1);
        });
    }
    {
        std::jthread thread ([]()
        {
            // Instance 3 of the variable.
            thread_local_var++;
            assert(thread_local_var == 1);
        });
    }
    assert(thread_local_var == 1);
}
}


namespace integer_sequence {
// A compile-time sequence of integers as a pack.

constexpr auto seq = std::integer_sequence<int, 1, 2, 3>();

static_assert(seq.size() == 3);

template <typename T, T... vals>
std::vector<T> assemble(std::integer_sequence<T, vals...> is) {
    std::vector<T> v;
    (void(v.emplace_back(vals)), ...);
    return v;
}

void demo() {
    const auto standard = std::vector<int>{1, 2, 3};
    assert(assemble(seq) == standard);
}
}


namespace demo_any {
void demo()
{
    // Holds any type.
    std::any any;
    any = 1;
    any = 1.0f;
    any = true;
    any = 'c';

    // Bad cast.
    try
    {
        any = 1;
        std::any_cast<float>(any);
    }
    catch (const std::bad_any_cast& e)
    {
        assert (e.what() == std::string("bad any cast"));
    }

    // Has value.
    any = 1;
    assert(any.has_value());

    // Reset.
    any.reset();
    assert(not any.has_value());

    // Pointer to contained data.
    any = 1;
    int* ip = std::any_cast<int>(&any);
    assert(*ip == 1);

    // Make any.
    auto any2 = std::make_any<std::string>("test");
    assert(std::any_cast<std::string>(any2) == "test");

    // Store lambda into std::any and run it.
    typedef std::function<void()> lambda;
    auto any3 = std::make_any<lambda>([&any] (){any = std::string("lambda"); });
    std::any_cast<lambda>(any3)();
    assert(std::any_cast<std::string>(any) == "lambda");
}
}


namespace demo_as_const {
void demo()
{
    std::string s1 = "test";
    const std::string& s2 = std::as_const(s1);
    assert(std::addressof(s2) == std::addressof(s1));
    using s2type = std::remove_reference_t<decltype(std::as_const(s1))>;
    static_assert(std::is_same_v<std::remove_const_t<s2type>, std::string>, "should be some string");
    static_assert(not std::is_same_v<s2type, std::string>, "should not be mutable");
}
}


namespace byte {
// The std::byte models a collection of bits, supports bitshift operations with an integer, and other bit operations.
void demo()
{
    // std::byte y = 1; // error: cannot convert int to byte
    std::byte b{1}; // OK.

    // if (b == 2){} // error: cannot compare
    if (b == std::byte{2}) {} // OK.

    int arr[] = {1, 2, 3};
    // int c = arr[b]; // error: array subscript is not an integer
    int i1 = arr[std::to_integer<int>(b)]; // OK
    int i2 = arr[std::to_underlying(b)];   // OK

    auto to_int = [](std::byte b) {return std::to_integer<int>(b);};

    b = std::byte{42};
    assert(to_int(b) == 0b00101010);

    // b *= 2; // error: b is not of arithmetic type
    b <<= 1; // OK.
    assert(to_int(b) == 0b01010100);

    b >>= 1;
    assert(to_int(b) == 0b00101010);

    b |= std::byte{0b11110000};
    assert(to_int(b) == 0b11111010);

    b &= std::byte{0b11110000};
    assert(to_int(b) == 0b11110000);

    b ^= std::byte{0b11111111};
    assert(to_int(b) == 0b00001111);
}
}


namespace conjunction_disjunction_negation {
// The std::conjunction perform a logical AND on a variadic pack of boolean values.
// The std::disjunction does a logical OR on a variadic pack of booleans.

bool all_the_same {};

// The function is enabled if all types Ts... are the same as type T.
template <typename T, typename ...Ts>
std::enable_if_t<std::conjunction_v<std::is_same<T, Ts>...>>
func(T, Ts...)
{
    all_the_same = true;
}

// The function is enabled if all types Ts... are not the same as type T.
template <typename T, typename ...Ts>
std::enable_if_t<not std::conjunction_v<std::is_same<T, Ts>...>>
func(T, Ts...)
{
    all_the_same = false;
}

template <typename T, typename ...Ts>
constexpr bool all_types_the_same = std::conjunction_v<std::is_same<T, Ts>...>;
static_assert(all_types_the_same<int, int, int>);
static_assert(not all_types_the_same<int, int, float>);

void demo_conjunction()
{
    func(1, 2, 3);
    assert(all_the_same);
    func(1, 2, 3.0f);
    assert(not all_the_same);
}


// Define two integral constants: 2 and 4.
using int2 = std::integral_constant<int, 2>;
using int4 = std::integral_constant<int, 4>;

// Takes the first element that is true, and returns its value:
static_assert(std::disjunction<int2, int4>::value == 2);
// Returns true based on the first element:
static_assert(std::disjunction_v<int2, int4>);

void demo_disjunction()
{
}


// The negation gives the logical negation of the passed type.

static_assert(
    std::is_same_v<
        std::bool_constant<false>,
        std::negation<std::bool_constant<true>>::type
    >,
    "");

static_assert(std::negation_v<std::bool_constant<false>>);

void demo_negation()
{
}

void demo()
{
    demo_conjunction();
    demo_disjunction();
    demo_negation();
}
}


namespace is_invocable {

auto func(char) -> int (*)()
{
    return nullptr;
}

static_assert(std::is_invocable_r_v<int(*)(), decltype(func), char>);
static_assert(not std::is_invocable_r_v<int(*)(), decltype(func), void>);

void demo() {}
}


namespace is_aggregate {
// An aggregate is a type that allows direct member initialization using curly braces ({})
// because it has no user-declared constructors and so on.

struct Aggregate
{
    int a;
    int b;
};
static_assert(std::is_aggregate_v<Aggregate>);

struct NonAggregate
{
    int i;
    std::string_view sv;
    NonAggregate (int i, std::string_view sv) : i(i), sv(sv) {}
};
static_assert(not std::is_aggregate_v<NonAggregate>);

void demo()
{
}
}


namespace has_unique_object_representations {

// Every value of a char corresponds to exactly one object representation.
static_assert(std::has_unique_object_representations_v<char>);

// IEC 559 floats: The value NaN has multiple object representations.
static_assert(not std::has_unique_object_representations_v<float>);

struct Unpadded { std::uint32_t a, b; };
// Should succeed in a sane implementation because struct unpadded
// is typically not padded, and std::uint32_t cannot contain padding bits.
static_assert(std::has_unique_object_representations_v<Unpadded>);

struct Padded { std::uint8_t u8; std::uint16_t u16; std::uint32_t u32; };
// Fails in most implementations because padding bits are inserted
// between the data members u8 and u16 for the purpose of aligning u16 to 16 bits.
static_assert(not std::has_unique_object_representations_v<Padded>);

// Notable architectural divergence:
static_assert(std::has_unique_object_representations_v<bool>);  // x86
// static_assert(not std::has_unique_object_representations_v<bool>); // ARM

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
    variant::demo();
    initialization::demo();
    designated_initializers::demo();
    mathematical_functions::demo();
    enable_shared_from_this::demo();
    swapping_and_exchanging::demo();
    initializer_list::demo();
    pseudo_random_number_generation::demo();
    heterogenous_collections_with_variant::demo();
    range_fillup::demo();
    aliases::demo();
    perfect_forwarding::demo();
    storage_duration::demo();
    integer_sequence::demo();
    demo_any::demo();
    demo_as_const::demo();
    byte::demo();
    conjunction_disjunction_negation::demo();
    is_invocable::demo();
    is_aggregate::demo();
    has_unique_object_representations::demo();
}


}
