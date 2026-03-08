#include <iostream>
#include <mutex>

#include "functions.h"

// If a normal mutex cannot be obtained, this leads to a deadlock.
// A timed mutex will assist in such a case.
// If a lock is requested on a timed mutex, a timeout can be passed too.
// If the lock cannot be obtained in time, it falls in a timeout, not in a deadlock.

namespace concurrency_support_library::mutual_exclusion {

static std::timed_mutex the_mutex;

void timed_mutex()
{
    std::cout << "Obtain first lock on the timed mutex" << std::endl;
    const std::unique_lock lock1(the_mutex, std::chrono::seconds(1));
    std::cout << "Obtained first lock" << std::endl;
    std::cout << "Obtain second lock on the same timed mutex" << std::endl;
    const std::unique_lock lock2(the_mutex, std::chrono::seconds(1));
    if (!lock2) {
        std::cout << "Failed to obtain second lock within one second" << std::endl;
    } else {
        std::cout << "Obtained second lock" << std::endl;
    }
}

}