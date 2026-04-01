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
#include <ranges>
#include <vector>
#include "lower_upper_bound.h"

// Demonstrate lower_bound and upper_bound.
namespace lower_bound_and_upper_bound {
constexpr auto container = std::array<int, 7>{2, 2, 3, 3, 3, 4, 5};
// The lower_bound returns an iterator to the first element not less than the given value.
static_assert(std::ranges::lower_bound(container, 3) != container.cend());
// The upper_bound returns an iterator to the first element greater than a certain value.
static_assert(std::ranges::upper_bound(container, 3) != container.cend());
void demo()
{
    {
        // The iterator is always valid because if no matching value is found,
        // the iterator will point to the last element of the range.
        auto it = std::ranges::lower_bound(container, 3);
        auto index = std::distance(container.begin(), it);
        assert(index == 2);
    }
    {
        auto it = std::ranges::upper_bound(container, 3);
        auto index = std::distance(container.begin(), it);
        assert(index == 5);
    }
}
}
