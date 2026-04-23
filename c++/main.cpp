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


namespace demo_ranges_various {
void demo()
{
    {
        // ranges::transform.
        const auto input = std::vector{1, 2, 3, 4};
        auto output = std::vector<int>(input.size());
        auto square = [](auto&& i) -> int { return i * i; };
        std::ranges::transform(input, output.begin(), square);
        // The output will be: 1 4 9 16
        const auto standard = std::vector<int>{1, 4, 9, 16};
        assert(output == standard);
    }
    {
        // ranges::views::transform.
        const auto input = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        const auto square = [](auto i) { return i * i; };
        // Create a view, but do not yet evaluate this view.
        auto squared_view = std::ranges::views::transform(input, square);
        // Iterate over the squared view, which invokes evaluation and so invokes the lambda.
        int sum{0};
        for (auto s : squared_view)
            sum += s;
        // The transformed output will be: 1 4 9 16 25 36 49 64 81 100
        assert(sum == 385);
    }
    {
        // Demonstration of ranges fill.
        std::vector<int> v(5);
        std::ranges::fill(v, 123);
        // The output will be:  123 123 123 123 123
        const std::vector<int> standard = {123, 123, 123, 123, 123};
        assert(v == standard);
    }
    {
        // Demonstration of ranges generate.
        std::vector<int> v(4);
        std::ranges::generate(v, std::rand);
        // The output will be random, e.g. 16807 282475249 1622650073 984943658
    }
    {
        // Demonstration of ranges iota.
        auto v = std::list<int>(6);
        // Fill the list with ascending values: 0, 1, 2, ...
        std::iota(v.begin(), v.end(), 0);
        const auto standard = std::list<int>({0, 1, 2, 3, 4, 5});
        assert(v == standard);
        //std::ranges::iota(v, 0);

        // A vector of iterators.
        // Fill with iterators to consecutive list's elements.
        std::vector<std::list<int>::iterator> vec(v.size());
        std::iota(vec.begin(), vec.end(), v.begin());
        //std::ranges::iota(vec.begin(), vec.end(), list.begin());
    }
    {
        // Demo of shuffle.
        // Numbers in sequential order.
        auto numbers = std::vector{1, 2, 3, 4, 5};
        assert(std::ranges::is_sorted(numbers));

        // A Mersenne Twister generator.
        std::random_device rd;
        std::mt19937 mtg(rd());

        // Shuffle the numbers.
        std::ranges::shuffle(numbers, mtg);

        // The output is in random order, e.g. 2 4 3 5 1.
        assert(!std::ranges::is_sorted(numbers ));
    }

    // Simple find integer value.
    {
        const auto numbers = std::list{2, 4, 3, 2, 3, 1};
        if (const auto it = std::ranges::find(numbers, 2);
            it != numbers.cend())
            assert(*it == 2);
    }

    // Find by specifying to search on a struct member.
    {
        struct person
        {
            unsigned id;
            const char* name;
            const char* job;
        };
        const auto persons = std::list<person>{
            {0, "Ana", "barber"},
            {1, "Bob", "cook"},
            {2, "Eve", "builder"}
        };
        if (const auto it = std::ranges::find(persons, "Bob", &person::name);
            it != persons.cend())
        {
            assert(it->id == 1);
            assert(it->name == std::string("Bob"));
            assert(it->job == std::string("cook"));
        }
    }

    // Demo of find on word length.
    {
        const auto names = std::vector<std::string>{"Apu", "Lisa", "Bart", "Ralph", "Homer", "Maggie"};
        const auto iterator = std::ranges::find(names, 4, &std::string::size);
        assert(iterator != names.cend());
        assert(*iterator == "Lisa");
    }


    // Demo of finding duplicates in a range.
    {
        const auto contains_duplicates_n2 = [](auto begin, auto end)
        {
            for (auto it = begin; it != end; ++it)
                if (std::find(std::next(it), end, *it) != end)
                    return true;
            return false;
        };
        const auto contains_duplicates_allocating = [](auto first, auto last)
        {
            // As (*first) returns a reference, we have to get the base type using std::decay_t
            using value_type = std::decay_t<decltype(*first)>;
            auto copy = std::vector<value_type>(first, last);
            std::sort(copy.begin(), copy.end());
            // The std::adjacent_find searches the sorted range for two consecutive equal elements.
            return std::adjacent_find(copy.begin(), copy.end()) != copy.end();
        };
        const auto vals = std::vector{1, 4, 2, 5, 3, 6, 4, 7, 5, 8, 6, 9, 0};
        const auto a = contains_duplicates_n2(vals.cbegin(), vals.cend());
        assert(a);
        const auto b = contains_duplicates_allocating(vals.cbegin(), vals.cend());
        assert(b);
    }

    // Demo of ranges::find_if
    {
        const auto values = {4, 1, 3, 2};
        const auto is_even = [](int x) { return x % 2 == 0; };
        const auto iter = std::ranges::find_if(values, is_even);
        assert(iter != values.end());
        assert(*iter == 4);
    }

    // Getting the maximum value.
    {
        struct Student
        {
            int year{};
            int score{};
            std::string name{};
        };

        auto get_max_score_copy = [](const std::vector<Student>& students, int year)
        {
            const auto by_year = [year](const auto& s) { return s.year == year; };
            // The student list needs to be copied in order to filter on the year.
            auto v = std::vector<Student>{};
            std::ranges::copy_if(students, std::back_inserter(v), by_year);
            auto it = std::ranges::max_element(v, std::less{}, &Student::score);
            return it != v.end() ? it->score : 0;
        };

        auto get_max_score = [](const std::vector<Student>& students, int year)
        {
            auto max_value = [](auto&& range)
            {
                const auto it = std::ranges::max_element(range);
                return it != range.end() ? *it : 0;
            };
            const auto by_year = [year](auto&& s) { return s.year == year; };
            return max_value(students | std::views::filter(by_year) | std::views::transform(&Student::score));
        };

        auto get_max_score_explicit_views = [](const std::vector<Student>& s, int year)
        {
            auto by_year = [year](const auto& s) { return s.year == year; };
            const auto v1 = std::ranges::ref_view{s}; // Wrap container in a view.
            const auto v2 = std::ranges::filter_view{v1, by_year};
            auto v3 = std::ranges::transform_view{v2, &Student::score};
            auto it = std::ranges::max_element(v3);
            return it != v3.end() ? *it : 0;
        };

        const auto students = std::vector<Student>{
            {3, 120, "A"},
            {2, 140, "B"},
            {3, 190, "C"},
            {2, 110, "D"},
            {2, 120, "E"},
            {3, 130, "F"},
        };
        {
            auto score = get_max_score_copy(students, 2);
            assert(score == 140);
        }
        {
            auto score = get_max_score(students, 2);
            assert(score == 140);
        }
        {
            auto score = get_max_score_explicit_views(students, 2);
            assert(score == 140);
        }
    }

    // Demo of join.
    {
        const auto list_of_lists = std::vector<std::vector<int>>{{1, 2}, {3, 4, 5}, {5}, {4, 3, 2, 1}};
        const auto flattened_view = std::views::join(list_of_lists);
        std::vector<int> result = flattened_view | std::ranges::to<std::vector<int>>();
        // Result: 1 2 3 4 5 5 4 3 2 1
        const auto standard = std::vector<int>{1, 2, 3, 4, 5, 5, 4, 3, 2, 1};
        assert(result == standard);
    }

    // Demo of take.
    {
        auto vec = std::vector{4, 2, 7, 1, 2, 6, 1, 5};
        // Create a view of the first half of the container.
        const auto first_half = vec | std::views::take(vec.size() / 2);
        // This sorts the first half of the original vector via the view.
        std::ranges::sort(first_half);
        const auto standard = std::vector{1, 2, 4, 7, 2, 6, 1, 5};
        assert(vec == standard);
    }

    // Demo of split and join.
    {
        // The input CSV data.
        auto csv = std::string{"10,11,12"};
        auto digits = csv | std::ranges::views::split(',');
        // Result:  [ [1, 0], [1, 1], [1, 2] ]
        auto joined = digits | std::views::join;
        // Result [ 1, 0, 1, 1, 1, 2 ]
        auto result = joined | std::ranges::to<std::vector<char>>();
        std::vector standard = {'1', '0', '1', '1', '1', '2'};
        assert(result == standard);
    }

    // Demo of drop_while.
    {
        auto vec = std::vector{1, 2, 3, 4, 5, 4, 3, 2, 1};
        // Apply the range adaptor that represents view of elements from an underlying sequence,
        // beginning at the first element for which the predicate returns false.
        auto v = vec | std::views::drop_while([](auto i) { return i < 5; });
        // Result: 5 4 3 2 1
        auto result= v | std::ranges::to<std::vector<int>>();
        auto standard = std::vector<int>{5, 4, 3, 2, 1};
        assert(result == standard);
    }

    // Demo of istream_view.
    {
        const auto s{"1.4142 1.618 2.71828 3.14159 6.283"};
        auto iss = std::istringstream{s};
        auto floats = std::ranges::istream_view<float>(iss);
        auto result = floats | std::ranges::to<std::vector<float>>();
        std::vector<float> standard = {1.4142, 1.618, 2.71828, 3.14159, 6.283};
        assert(result == standard);
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
    demo_ranges_various::demo();
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

