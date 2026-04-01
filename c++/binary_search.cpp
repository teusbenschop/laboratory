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

#include "binary_search.h"

// Demonstrate binary_search.
// A binary search works if the container is sorted.
// It returns true if the value is found in the container.
// It is also possible to pass a comparator.

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
