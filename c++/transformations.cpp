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

#include "transformations.h"

#include <array>
#include <cassert>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

namespace transformations {

namespace accumulate {
constexpr std::array<int, 3> integers{1, 2, 3};
constexpr auto sum = std::accumulate(integers.cbegin(), integers.cend(), 0);
static_assert(sum == 6);
constexpr auto product = std::accumulate(integers.cbegin(), integers.cend(), 1, std::multiplies<int>());
static_assert(product == 6);

void demo()
{
    const auto strings = std::vector<std::string>{"a", "b"};
    const std::string init{"init"};
    const auto concat = std::accumulate(strings.cbegin(), strings.cend(), init);
    assert(concat == "initab");

    const auto dash_fold = [](std::string a, int b)
    {
        return std::move(a) + '-' + std::to_string(b);
    };
    std::string s = std::accumulate(std::next(integers.begin()), integers.end(),
                                    std::to_string(integers.at(0)), // start with first element
                                    dash_fold);
    assert(s == "1-2-3");
}
}


namespace copy {
void demo()
{
    constexpr auto values = std::array<int, 3>{1, 2, 3};
    {
        std::vector<int> copy{};
        std::copy(values.begin(), values.end(), std::back_inserter(copy));
        assert(copy.size() == 3);
    }
    {
        std::vector<int> copy{};
        std::ranges::copy(values, std::back_inserter(copy));
        assert(copy.size() == 3);
    }
}
}


namespace ranges_views_filter_drop_reverse {
void demo()
{
    // Start from a list of numbers.
    const auto numbers = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    // Define a function to select even numbers.
    const auto even = [](const int i) -> bool
    {
        return 0 == i % 2;
    };
    // Define a view that filters on even numbers, then drops the first number, then reverses them.
    using namespace std::ranges::views;
    auto result = numbers | filter(even) | drop(1) | reverse;
    // The view contains: 8 6 4 2
    for (const int i : result)
        assert(i == 8 or i == 6 or i == 4 or i == 2);
}
}





void demo()
{
    accumulate::demo();
    copy::demo();
    ranges_views_filter_drop_reverse::demo();
}
}
