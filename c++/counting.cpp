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
#include <string>
#include <vector>
#include "counting.h"

namespace counting {

constexpr auto numbers = std::array{3, 3, 2, 1, 3, 1, 3};

// Count how often a given number occurs.
static_assert(std::ranges::count(numbers, 3) == 4);

constexpr auto predicate = [](const int i) { return i == 3; };
static_assert(std::ranges::count_if(numbers, predicate) == 4);

void demo()
{
    {
        struct Item {
            std::string name;
            int id;
        };
        constexpr auto items = std::to_array<Item>({
            {.name = "Foo", .id = 1},
            {.name = "Bar", .id = 2},
            {.name = "Foo", .id = 3}
        });
        // Count how many items have the name "Foo".
        constexpr long count = std::ranges::count(items, "Foo", &Item::name);
        static_assert(count == 2);
    }

    {
        using pair_t = std::pair<int, int>;
        constexpr auto pairs = std::to_array<pair_t>({{1,1}, {1,2}, {3,3}});
        // Count how many pairs have 1 as their first value.
        constexpr long count = std::ranges::count(pairs, 1, &pair_t::first);
        static_assert(count == 2);
    }
}

}
