#include "functions.h"

namespace language {

// Template with a default type.
template<typename T = int>
auto sum(T a, T b) -> T
{
    return a + b;
}


void demo_templates()
{

}

}