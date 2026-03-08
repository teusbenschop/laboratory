#include <iostream>

#include "concurrency_support_library/mutual_exclusion/functions.h"
#include "language_support_library/program_utilities/functions.h"
#include "metaprogramming_library/functions.h"
#include "ranges_library/functions.h"
#include "language/functions.h"
#include "memory_management_library/functions.h"
#include "ranges_library/functions.h"
#include "concurrency_support_library/mutual_exclusion/functions.h"

int main() {
    // language_support_library::program_utilities::signals();
    // ranges_library::introduction();
    // ranges_library::various_demos();
    // metaprogramming_library::automatic_unit_conversion();
    // language::exceptions();
    // language::classes::design_principles();
    // language::classes::design_idioms();
    // memory_management_library::pointer_types();
    // concurrency_support_library::mutual_exclusion::shared_and_unique_locks();
    //concurrency_support_library::mutual_exclusion::timed_mutex();
    // concurrency_support_library::mutual_exclusion::demo_latch();
    concurrency_support_library::mutual_exclusion::demo_barrier();
    return EXIT_SUCCESS;
}