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


#include <algorithm>
#include <array>
#include <cassert>
#include <complex>
#include <ranges>
#include <vector>

#include "searching.h"

// Demonstrate binary_search.
// A binary search works if the container is sorted.
// It returns true if the value is found in the container.
// It is also possible to pass a comparator.

namespace searching {

namespace binary_search {
constexpr auto container = std::array<int, 7>{2, 2, 3, 3, 3, 4, 5};
static_assert(std::ranges::is_sorted(container));
static_assert(std::ranges::binary_search(container, 3));

void demo()
{
    using ci = std::complex<int>;
    const std::vector<ci> complex_numbers = {
        {1, 1},
        {2, 3},
        {4, 2},
        {4, 3},
    };
    const auto comparator = [](const ci& a, const ci& b) {return std::abs(a) < std::abs(b);};
    assert(std::ranges::is_sorted(complex_numbers, comparator));
    assert(std::ranges::binary_search(complex_numbers, ci{4,2}, comparator));
}
}


// Demonstrate any_of / all_of / none_of.

namespace any_of_all_of_none_of {

// Input of positive numbers.
constexpr auto numbers = std::array<int, 7>{3, 2, 2, 1, 0, 2, 1};

// Function testing negative number.
constexpr auto is_negative = [](int i) { return i < 0; };

// None of the numbers is negative.
static_assert(std::ranges::none_of(numbers, is_negative));
static_assert(not std::ranges::all_of(numbers, is_negative));

// Not any of the numbers is negative.
static_assert(not std::ranges::any_of(numbers, is_negative));

void demo()
{
}

}


namespace starts_with_and_ends_with {
// https://en.cppreference.com/w/cpp/string/basic_string/starts_with
// https://en.cppreference.com/w/cpp/string/basic_string/ends_with
// https://en.cppreference.com/w/cpp/algorithm/ranges/starts_with
// https://en.cppreference.com/w/cpp/algorithm/ranges/ends_with.html
void demo()
{
    constexpr std::string_view hello_world{"hello world"};
    static_assert(hello_world.starts_with("hello"));
    static_assert(not hello_world.starts_with("world"));
    static_assert(hello_world.ends_with('d'));
    static_assert(not hello_world.ends_with("hello"));

    using namespace std::literals;
    static_assert(std::ranges::starts_with("const_cast", "const"sv));
    static_assert(std::ranges::starts_with("constexpr", "const"sv));
    static_assert(!std::ranges::starts_with("volatile", "const"sv));

    constexpr auto ascii_upper = [](char8_t c)
    {
        return u8'a' <= c && c <= u8'z' ? static_cast<char8_t>(c + u8'A' - u8'a') : c;
    };

    constexpr auto cmp_ignore_case = [=](char8_t x, char8_t y)
    {
        return ascii_upper(x) == ascii_upper(y);
    };

    // Projection 1: the projection to apply to the elements of the range to examine.
    // Projection 2: the projection to apply to the elements of the range to be used as the prefix.
    static_assert(std::ranges::starts_with(u8"Constantinopolis", u8"constant"sv,{}, ascii_upper, ascii_upper));
    static_assert(not std::ranges::starts_with(u8"Istanbul", u8"constant"sv,{}, ascii_upper, ascii_upper));
    // The predicate: the binary predicate that compares the projected elements.
    static_assert(std::ranges::starts_with(u8"Metropolis", u8"metro"sv,cmp_ignore_case));
    static_assert(not std::ranges::starts_with(u8"Acropolis", u8"metro"sv,cmp_ignore_case));

    constexpr static auto v = {1, 3, 5, 7, 9};
    constexpr auto odd = [](int x) { return x % 2; };
    assert(std::ranges::starts_with(v, std::views::iota(1) | std::views::filter(odd) | std::views::take(3)));

    static_assert(not std::ranges::ends_with("for", "cast"));
    static_assert(std::ranges::ends_with("dynamic_cast", "cast"));
    static_assert(not std::ranges::ends_with("as_const", "cast"));
    static_assert(std::ranges::ends_with("bit_cast", "cast"));
    static_assert(not std::ranges::ends_with("to_underlying", "cast"));
    static_assert(std::ranges::ends_with(std::array{1, 2, 3, 4}, std::array{3, 4}));
    static_assert(not std::ranges::ends_with(std::array{1, 2, 3, 4}, std::array{4, 5}));
}
}

namespace header_compare {
// https://en.cppreference.com/w/cpp/header/compare
void demo()
{
}
}



void demo() {
    binary_search::demo();
    any_of_all_of_none_of::demo();
    starts_with_and_ends_with::demo();
    header_compare::demo();
}


}
