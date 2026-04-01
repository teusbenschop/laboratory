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

#include <array>
#include <algorithm>
#include "any_of_all_of_none_of.h"


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