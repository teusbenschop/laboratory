

#include "main.h"
#include <memory>
#include "application.h"

// Instantiate the app, this also instantiates the main() function.
// This architecture is used in the ncurses++ demo from the ncurses++ sources.
namespace {
// Reduce visibility and increase encapsulation.
constexpr auto enable_colors = true;
[[maybe_unused]] const auto ums_configure = std::make_unique<Application>(enable_colors);
}
