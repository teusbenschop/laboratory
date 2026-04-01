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
#include "counting.h"

namespace counting {

constexpr auto numbers = std::array<int, 7>{3, 3, 2, 1, 3, 1, 3};

// Count how often number 3 occurs: Should be 4 times.
static_assert(std::ranges::count(numbers, 3) == 4);

constexpr auto predicate = [](int i) { return i == 3; };
static_assert(std::ranges::count_if(numbers, predicate) == 4);

void demo()
{
}

}
