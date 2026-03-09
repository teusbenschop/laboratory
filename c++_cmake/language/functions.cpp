#include <functional>
#include <iostream>

#include "functions.h"


namespace language {

void demo_lambda_capture()
{
    // Capture by value.
    {
        auto v = 7;
        // Note that the following lambda, via the capture, copies the current v = 7 into the lambda.
        // Mark it mutable because it mutates the captured v.
        // Note that it captures v once, and stores it in the lambda function.
        // Hence, the increased v is kept and can be increased once more and so on.
        auto lambda = [v] mutable {
            v++;
            std::cout << "Captured v by value = " << v << std::endl;
        };
        lambda();
        lambda();
        std::cout << "Original v = " << v << std::endl;
    }

    // Capture by reference.
    {
        auto v = 7;
        auto lambda = [&v]() {
            ++v;
            std::cout << "Captured v by reference = " << v << std::endl;
        };
        lambda();
        lambda();
        std::cout << "Original v = " << v << std::endl;
    }
}

void assign_two_lambdas_to_same_function_object ()
{
    // Create an unassigned std::function object.
    auto func = std::function<void(int)>{};

    // Assign a lambda without capture to the std::function object.
    func = [](int v) {
        std::cout << "Assigned lambda without capture: " << v << std::endl;
    };
    func(12); // Prints 12.

    // Assign a lambda with capture to the same std::function object.
    auto v42 = 42;
    func = [v42](int v) {
        std::cout << "Assigned lambda with capture: " << (v + v42) << std::endl;
    };
    func(12); // Prints 54.
}


// A stateless lambda (function) does not retain any data or memory
// from one execution to the next.
const auto stateless = [] {};
// It is assignable.
const auto stateless2 = stateless;
// Default-constructible (i.e. constructor without parameters, or with default parameters).
static_assert(std::is_default_constructible_v<decltype(stateless)>); // passes
const decltype(stateless) stateless3;
static_assert(std::is_same_v<decltype(stateless), decltype(stateless2)>); // passes
static_assert(std::is_same_v<decltype(stateless), decltype(stateless3)>); // passes
static_assert(std::is_same_v<decltype(stateless2), decltype(stateless3)>); // passes


// Using keyword "auto" for lambda functions.
constexpr auto lambda_typename = []<typename T>(T v) { return v + 1; };
constexpr auto lambda_auto = [](auto v) { return v + 1; };
static_assert(lambda_typename('a') == 'b');
static_assert(lambda_auto('a') == 'b');
static_assert(lambda_typename(1) == 2);
static_assert(lambda_auto(1) == 2);
static_assert(lambda_typename(static_cast<std::uint64_t>(41)) == 42);
static_assert(lambda_auto(static_cast<std::uint64_t>(41)) == 42);



}
