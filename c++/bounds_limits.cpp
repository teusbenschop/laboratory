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
#include <cassert>
#include <list>
#include <ranges>
#include <vector>
#include "bounds_limits.h"



namespace bounds_limits {
namespace lower_bound_and_upper_bound {

// The lower_bound returns an iterator to the first element >= the given value.
// The upper_bound returns an iterator to the first element >  the given value.
// It is comparable to .end(), which points one position "past" last element in a range.
// The iterator is always valid because if no matching value is found,
// the iterator will point to container.end().
constexpr auto container = std::array{2, 2, 3, 3, 3, 4, 5};
constexpr auto lower_iter = std::ranges::lower_bound(container, 3);
constexpr auto upper_iter = std::ranges::upper_bound(container, 3);
static_assert(*std::ranges::lower_bound(container, 3) == 3);
static_assert(*std::ranges::upper_bound(container, 3) == 4);
static_assert(std::distance(container.begin(), lower_iter) == 2);
static_assert(std::distance(container.begin(), upper_iter) == 5);

static void demo()
{
}
}


// Demonstrate: min / max / clamp / ranges::minmax.
namespace min_max_clamp_ranges_minmax {
constexpr auto i200 = []() { return 200; };
constexpr auto i10 = 10;
constexpr auto i100 = 100;
static_assert(std::min(i200(), i100) == i100);
static_assert(std::max(i200(), i10) == i200());
static_assert(std::clamp(i200(), i10, i100) == i100);

static void demo()
{
    const std::list values{4, 2, 1, 7, 3, 1, 5};
    const auto [min, max] = std::ranges::minmax(values);
    assert(min == 1);
    assert(max == 7);
}
}

namespace numerical_limits {

static_assert(not std::numeric_limits<unsigned>::is_signed);
static_assert(std::numeric_limits<long>::is_integer);

static void demo()
{
}
}

void demo()
{
    lower_bound_and_upper_bound::demo();
    min_max_clamp_ranges_minmax::demo();
    numerical_limits::demo();
}
}
