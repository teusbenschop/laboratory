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
#include <barrier>
#include <cassert>
#include <chrono>
#include <complex>
#include <condition_variable>
#include <cstdlib>
#include <forward_list>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <latch>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <ranges>
#include <regex>
#include <source_location>
#include <syncstream>
#include <thread>
#include <vector>

#include "bad_coding.h"
#include "bits.h"
#include "bounds_limits.h"
#include "classes.h"
#include "clocking.h"
#include "concurrency.h"
#include "constraints.h"
#include "containers.h"
#include "coroutines.h"
#include "counting.h"
#include "exceptions.h"
#include "expected.h"
#include "filesystem.h"
#include "fold_expressions.h"
#include "functional.h"
#include "language.h"
#include "latches_barriers.h"
#include "linux.h"
#include "modules.h"
#include "searching.h"
#include "shared_mutex.h"
#include "templates.h"
#include "text.h"
#include "transformations.h"
#include "variables.h"







namespace ranges_sorting_demo {
void demo()
{
    {
        // Standard sorting demo.
        auto values = std::vector{6, 3, 2, 7, 4, 1, 5};
        assert(!std::ranges::is_sorted(values));
        // Regular C++ sort: std::sort(non_const_values.begin(), non_const_values.end());
        std::ranges::sort(values);
        const auto standard = std::vector{1, 2, 3, 4, 5, 6, 7};
        assert(values == standard);
        assert(std::ranges::is_sorted(values));
    }
    {
        // Demo of sorting on property, in this case, word length.
        auto names = std::vector<std::string>{"Ralph", "Lisa", "Homer", "Maggie", "Apu", "Bart"};
        std::ranges::sort(names, std::less{}, &std::string::size);
        // The names are now sorted on length: "Apu", "Lisa", "Bart", "Ralph", "Homer", "Maggie"
    }
    {
        // Demo of sorting a struct on two fields.
        struct Player
        {
            std::string name{};
            int level{};
            float health{};
        };
        auto players = std::vector<Player>{
            {"Aki", 1, 9.1f},
            {"Nao", 2, 7.2f},
            {"Rei", 2, 3.3f}
        };
        const auto level_and_health = [](const Player& player)
        {
            return std::tie(player.level, player.health);
        };
        // Order players by level, then by health.
        std::ranges::sort(players, std::less<>{}, level_and_health);
        assert(players.at(0).name == "Aki");
        assert(players.at(1).name == "Rei");
        assert(players.at(2).name == "Nao");
    }
}
}



int main()
{
    scoped_timer::demo();
    searching::demo();
    lower_bound_and_upper_bound::demo();
    counting::demo();
    min_max_clamp_ranges_minmax::demo();
    latches::demo();
    barriers::demo();
    unconstrained_errors::demo();
    constraint_derived_from::demo();
    demonstrate_constraints::demo();
    shared_mutex::demo();
    clocking::demo();
    filesystem::demo();
    ranges_sorting_demo::demo();
    coroutines::demo();
    modules::demo();
    concurrency::demo();
    fold_expressions::demo();
    containers::demo();
    language::demo();
    templates::demo();
    variables::demo();
    functional::demo();
    bad_coding::demo();
    expected::demo();
    text::demo();
    classes::demo();
    exceptions::demo();
    transformations::demo();
    bits::demo();
    linux::demo();

    return EXIT_SUCCESS;
}

