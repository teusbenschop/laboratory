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

#include "functional.h"

#include <cassert>
#include <functional>
#include <future>
#include <iostream>

namespace functional {


namespace move_only_function {
// https://en.cppreference.com/w/cpp/utility/functional/move_only_function.html
// C++23 introduces the std::move_only_function,
// a new utility for handling callable objects that don't need to be copyable.
// It's a streamlined alternative to std::function,
// designed for cases where you're working with move-only types
// like std::unique_ptr or other non-copyable resources.
void demo()
{
    return;
    std::packaged_task<double()> packaged_task([](){ return 3.14159; });

    std::future<double> future = packaged_task.get_future();

    auto lambda = [task = std::move(packaged_task)]() mutable { task(); };

    //  std::function<void()> function = std::move(lambda); // Error

    // std::move_only_function<void()> function = std::move(lambda); // OK does not yet compile on macOS 15.4.1

    //function();
    lambda();

    std::cout << future.get();

}
}



namespace brackets_are_optional_for_lambdas {
void demo()
{
    std::string s1 = "s1";
    auto with_parenthesis = [s1 = std::move(s1)] () {
        assert(not s1.empty());
    };
    with_parenthesis();

    std::string s2 = "s2";
    auto without_parenthesis = [s2 = std::move(s2)]  {
        assert(not s2.empty());
    };
    without_parenthesis();
}
}


namespace lambda_capture {
void demo()
{
    // Capture by value.
    {
        int captured_v_by_value;
        auto v = 7;
        // Note that the following lambda, via the capture, copies the current v = 7 into the lambda.
        // Mark it mutable because it mutates the captured v.
        // Note that it captures v once, and stores it in the lambda function.
        // Hence, the increased v is kept and can be increased once more and so on.
        auto lambda = [v](int& captured_v_by_value) mutable
        {
            v++;
            captured_v_by_value = v;
        };
        lambda(captured_v_by_value);
        assert(captured_v_by_value == v + 1);
        lambda(captured_v_by_value);
        assert(captured_v_by_value == v + 2);
        // The original v remains unchanged.
        assert(v == 7);
    }

    // Capture by reference.
    {
        auto v = 7;
        auto lambda = [&v]
        {
            ++v;
        };
        lambda();
        assert(v == 8);
        lambda();
        assert(v == 9);
    }
}
}


namespace assign_two_lambdas_to_same_function_object {
void demo()
{
    return;
    // Create an unassigned std::function object.
    std::function<void(int)> func = nullptr;

    // Assign a lambda without capture to the std::function object.
    func = [](int v)
    {
        std::cout << "Assigned lambda without capture: " << v << std::endl;
    };
    func(12); // Prints 12.

    // Assign a lambda with capture to the same std::function object.
    auto v42 = 42;
    func = [v42](int v)
    {
        std::cout << "Assigned lambda with capture: " << (v + v42) << std::endl;
    };
    func(12); // Prints 54.
}
}


namespace stateless_lambda_function {
// A stateless lambda function does not retain any data or memory
// from one execution to the next.
constexpr auto stateless1 = []
{
};
// It is assignable.
constexpr auto stateless2 = stateless1;
// Default-constructible (i.e. constructor without parameters, or with default parameters).
static_assert(std::is_default_constructible_v<decltype(stateless1)>); // passes
constexpr decltype(stateless1) stateless3;
static_assert(std::is_same_v<decltype(stateless1), decltype(stateless2)>); // passes
static_assert(std::is_same_v<decltype(stateless1), decltype(stateless3)>); // passes
static_assert(std::is_same_v<decltype(stateless2), decltype(stateless3)>); // passes
void demo()
{
}
}


namespace keyword_auto_for_lambdas {
// Can use "typename T" or "auto" for lambda functions.
constexpr auto lambda_typename = []<typename T>(T value) -> T {
    T val2 = value;
    decltype(value) val3 = value;
    return value + 1;
};
constexpr auto lambda_auto = [](auto v) -> auto { return v + 1; };
static_assert(lambda_typename('a') == 'b');
static_assert(lambda_auto('a') == 'b');
static_assert(lambda_typename(1) == 2);
static_assert(lambda_auto(1) == 2);
static_assert(lambda_typename(static_cast<std::uint64_t>(41)) == 42);
static_assert(lambda_auto(static_cast<std::uint64_t>(41)) == 42);
void demo(){}
}


namespace binding {
// Demo of std::bind, including bind_front and bind_back.
// https://cppreference.com/w/cpp/utility/functional/bind.html
void demo()
{
    constexpr auto minus = [](int a, int b) -> int
    {
        return a - b;
    };

    struct Foo
    {
        constexpr int minus(int a, int b) const noexcept
        {
            return a - b;
        }

        int val10 = 10;
    };

    // Struct instance.
    Foo foo;

    // The function template std::bind generates a forwarding call wrapper for f.
    // Calling this wrapper is equivalent to invoking f with some of its arguments bound to args.

    const auto value1_minus_value2 = std::bind(minus, std::placeholders::_1, std::placeholders::_2);
    assert(value1_minus_value2(1, 2) == -1);

    const auto value_minus_3 = std::bind(minus, std::placeholders::_1, 3);
    assert(value_minus_3(1) == -2);

    // Bind to a pointer to a member function.
    const auto value1_minus_value2_via_member_fn = std::bind(&Foo::minus, &foo, std::placeholders::_1,
                                                             std::placeholders::_2);
    assert(value1_minus_value2_via_member_fn(1, 2) == -1);

    // Bind to a pointer to a data member.
    const auto get_value_from_struct = std::bind(&Foo::val10, std::placeholders::_1);
    assert(get_value_from_struct(foo) == 10);

    // Function templates std::bind_front and std::bind_back
    // generate a perfect forwarding call wrapper
    // which allows to invoke the callable target
    // with its first or last sizeof...(Args) parameters bound to args.

    const auto fifty_minus_value = std::bind_front(minus, 50);
    assert(fifty_minus_value(3) == 47); // equivalent to `minus(50, 3)`: 47.

    const auto value_minus_fifty = std::bind_back(minus, 50);
    assert(value_minus_fifty(5) == -45);
}
}


namespace member_function {
// Function template std::mem_fn generates wrapper objects for pointers to members.
// It can store, copy, and invoke a pointer to member.

struct Foo
{
    std::string display_greeting()
    {
        return "hello";
    }

    int display_number(int i)
    {
        return i;
    }

    int add_xy(int x, int y)
    {
        return data + x + y;
    }

    template <typename... Args>
    int add_many1(Args... args)
    {
        return data + (args + ...);
    }
x
    auto add_many2(auto... args) // C++20 required
    {
        return data + (args + ...);
    }

    int data = 7;
};

void demo()
{
    auto f = Foo{};

    constexpr auto greet = std::mem_fn(&Foo::display_greeting);
    assert(greet(f) == "hello");

    const auto print_num = std::mem_fn(&Foo::display_number);
    assert(print_num(f, 42) == 42);

    const auto access_data = std::mem_fn(&Foo::data);
    assert(access_data(f) == 7);

    const auto add_xy = std::mem_fn(&Foo::add_xy);
    assert(add_xy(f, 1, 2) == 10);

    const auto u = std::make_unique<Foo>();
    assert(access_data(u) == 7);
    assert(add_xy(u, 1, 2) == 10);

    const auto add_many = std::mem_fn(&Foo::add_many1<short, int, long>);
    assert(add_many(u, 1, 2, 3) == 13);

    const auto add_them = std::mem_fn(&Foo::add_many2<short, int, float, double>);
    assert(add_them(u, 5, 7, 10.0f, 13.0) == 42);
}
}




void demo() {
    move_only_function::demo();
    brackets_are_optional_for_lambdas::demo();
    lambda_capture::demo();
    assign_two_lambdas_to_same_function_object::demo();
    stateless_lambda_function::demo();
    keyword_auto_for_lambdas::demo();
    binding::demo();
    member_function::demo();
}
}



