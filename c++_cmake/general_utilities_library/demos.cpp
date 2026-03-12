#include <cassert>
#include <iostream>
#include <tuple>
#include <utility>
#include <type_traits>

#include "functions.h"

namespace general_utilities_library {
void piecewise_construct_demo()
{
    enum Construction { from_tuple, from_int_float };

    struct Foo
    {
        Construction construction;

        // Constructor from a tuple.
        explicit Foo(std::tuple<int, float>)
        {
            construction = from_tuple;
        }

        // Constructor from an int and a float.
        explicit Foo(int, float)
        {
            construction = from_int_float;
        }
    };

    const std::tuple<int, float> tuple(1, 1.0f);

    // This creates a pair of Foo objects from a tuple.
    const std::pair<Foo, Foo> p1{tuple, tuple};
    assert(p1.first.construction == Construction::from_tuple);
    assert(p1.second.construction == Construction::from_tuple);

    // This creates the same as above but then piecewise, so from an int and a float.
    const std::pair<Foo, Foo> p2{std::piecewise_construct, tuple, tuple};
    assert(p2.first.construction == Construction::from_int_float);
    assert(p2.second.construction == Construction::from_int_float);
}


void forward_as_tuple_demo()
{
    // The helper "forward_as_tuple" takes a variadic pack of arguments
    // and creates a tuple out of them.
    // It determines the types of the elements of the arguments:
    // if it receives an lvalue then it will have an lvalue reference,
    // and if it receives an rvalue then it will have an rvalue reference.

    // Function returns an rvalue.
    const auto universe = [] -> std::string
    {
        return "universe";
    };

    // The variable i is a lvalue.
    int i = 42;

    [[maybe_unused]] auto tuple = std::forward_as_tuple(i, universe());

    // An lvalue reference binds to an lvalue. Marked with one ampersand (&).
    // An rvalue reference binds to an rvalue. Marked with two ampersands(&&)
    static_assert(std::is_same_v<decltype(tuple), std::tuple<int&, std::string&&>>);
}


void demos()
{
    piecewise_construct_demo();
    forward_as_tuple_demo();
}
}
