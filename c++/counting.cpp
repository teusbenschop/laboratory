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

#include "counting.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <ranges>
#include <string>
#include <vector>

namespace counting {

constexpr auto numbers = std::array{3, 3, 2, 1, 3, 1, 3};

// Count how often a given number occurs.
static_assert(std::ranges::count(numbers, 3) == 4);

constexpr auto predicate = [](int i) { return i == 3; };
static_assert(std::ranges::count_if(numbers, predicate) == 4);

void demo()
{
    {
        struct Player {
            std::string name;
            int score;
        };
        std::vector<Player> players{
            {"Alice", 10},
            {"Bob", 10},
            {"Alice", 10}
        };
        // Count how many players have the name "Alice".
        long count = std::ranges::count(players, "Alice", &Player::name);
        assert(count == 2);
    }

    {
        std::vector<std::pair<int, int>> pairs {{1,1}, {1,2}, {3,3} };
        long count = std::ranges::count(pairs, 1, &std::pair<int,int>::first);
        assert(count == 2);
    }
}

}
