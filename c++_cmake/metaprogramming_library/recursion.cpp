#include <iostream>
#include <ostream>
#include <type_traits>
#include "functions.h"

namespace metaprogramming_library {

// What is template metaprogramming?
// It means that the compiler, using templates, calculates values at compile time.
// Here is an example of recursion to let the compiler calculate values.

// A factorial of, say, 4 means: multiply all numbers from 4 down to 1.

// This template recursively calls itself to calculate its value.
template <int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value;
};

// This template is specific for a passed value of 0.
template <>
struct Factorial<0> {
    static constexpr int value = 1;
};

static_assert(Factorial<1>::value == 1);
static_assert(Factorial<4>::value == 24);
static_assert(Factorial<6>::value == 720);


// Another example of recursive template calls.
template<int N>
struct Power {
    enum { value = 2 * Power<N-1>::value };
};

template<>
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


}
