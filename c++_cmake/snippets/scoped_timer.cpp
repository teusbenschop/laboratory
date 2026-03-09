#include "scoped_timer.hpp"

#include "chrono"
#include <thread>

#include "functions.h"

namespace snippets {

void demo_scoped_timer()
{
    const auto timer = scoped_timer<std::chrono::microseconds>{};
    std::this_thread::sleep_for(std::chrono::microseconds(100));
}

}
