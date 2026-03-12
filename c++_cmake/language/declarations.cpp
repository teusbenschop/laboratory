
#include <type_traits>

#include "functions.h"

namespace language {

// The keyword "const" indicates immutability.
const int i1 = 10;

// Keyword "constexpr":
// If possible evaluate it at compile time.
constexpr int i2 = 10;

// Will be evaluated at compile time if the parameters "var1" and "var2" are known at compile time.
// Will be evaluated at run time if the parameters are not known.
constexpr int constexpr_add (int var1, int var2)
{
    return var1 + var2;
}

// consteval : Force evaluation of a function at compile time.
consteval int consteval_add (int var1, int var2)
{
    return var1 + var2;
}

// constinit : Force evaluation at compile time (but variable is not const).
constinit int i3 = 1;

void declarations()
{
    // Can all be evaluated at compile time.
    static_assert(i1 == 10);
    static_assert(i2 == 10);
    static_assert(constexpr_add(1,2) == 3);
    static_assert(constexpr_add(i1,i2) == 20);

    // Variable i3 is assigned at runtime.
    [[maybe_unused]]int i3 = 2;
    // Compile error: the value of 'i3' is not usable in a constant expression
    // static_assert(constexpr_add(i3,i3) == 4);

    // The variable is initialized at runtime although the function is constexpr.
    [[maybe_unused]] int i4 = constexpr_add(i3, i3);

    // OK, evaluated at compile-time.
    static_assert(consteval_add(1,1) == 2);

    // Cannot be evaluated at compile-time.
    // Compile error:
    // call to consteval function 'consteval_add(i3, i3)' is not a constant expression
    // int i5 = consteval_add(i3, i3);
    // Can all be evaluated at compile time.
    static_assert(i1 == 10);
    static_assert(i2 == 10);
    static_assert(constexpr_add(1,2) == 3);
    static_assert(constexpr_add(i1,i2) == 20);

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
    static_assert(if_constexpr_add(1,2) == 3);

    {
        constexpr const auto xdigit = [](int n) -> char {
            // This is a constexpr variable in a constexpr lambda function: OK in C++23.
            constexpr const char digits[] = "0123456789abcdef";
            return digits[n];
        };
        static_assert(xdigit(2) == '2');
    }


}

}