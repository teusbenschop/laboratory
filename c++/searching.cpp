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

#include "searching.h"

#include <iostream>
#include <list>
#include <regex>

#include "clocking.h"


namespace searching {


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


namespace starts_with_and_ends_with {
// https://en.cppreference.com/w/cpp/string/basic_string/starts_with
// https://en.cppreference.com/w/cpp/string/basic_string/ends_with
// https://en.cppreference.com/w/cpp/algorithm/ranges/starts_with
// https://en.cppreference.com/w/cpp/algorithm/ranges/ends_with.html
void demo()
{
    constexpr std::string_view hello_world{"hello world"};
    static_assert(hello_world.starts_with("hello"));
    static_assert(not hello_world.starts_with("world"));
    static_assert(hello_world.ends_with('d'));
    static_assert(not hello_world.ends_with("hello"));

    using namespace std::literals;
    static_assert(std::ranges::starts_with("const_cast", "const"sv));
    static_assert(std::ranges::starts_with("constexpr", "const"sv));
    static_assert(!std::ranges::starts_with("volatile", "const"sv));

    constexpr auto ascii_upper = [](char8_t c)
    {
        return u8'a' <= c && c <= u8'z' ? static_cast<char8_t>(c + u8'A' - u8'a') : c;
    };

    constexpr auto cmp_ignore_case = [=](char8_t x, char8_t y)
    {
        return ascii_upper(x) == ascii_upper(y);
    };

    // Projection 1: the projection to apply to the elements of the range to examine.
    // Projection 2: the projection to apply to the elements of the range to be used as the prefix.
    static_assert(std::ranges::starts_with(u8"Constantinopolis", u8"constant"sv,{}, ascii_upper, ascii_upper));
    static_assert(not std::ranges::starts_with(u8"Istanbul", u8"constant"sv,{}, ascii_upper, ascii_upper));
    // The predicate: the binary predicate that compares the projected elements.
    static_assert(std::ranges::starts_with(u8"Metropolis", u8"metro"sv,cmp_ignore_case));
    static_assert(not std::ranges::starts_with(u8"Acropolis", u8"metro"sv,cmp_ignore_case));

    constexpr static auto v = {1, 3, 5, 7, 9};
    constexpr auto odd = [](int x) { return x % 2; };
    assert(std::ranges::starts_with(v, std::views::iota(1) | std::views::filter(odd) | std::views::take(3)));

    static_assert(not std::ranges::ends_with("for", "cast"));
    static_assert(std::ranges::ends_with("dynamic_cast", "cast"));
    static_assert(not std::ranges::ends_with("as_const", "cast"));
    static_assert(std::ranges::ends_with("bit_cast", "cast"));
    static_assert(not std::ranges::ends_with("to_underlying", "cast"));
    static_assert(std::ranges::ends_with(std::array{1, 2, 3, 4}, std::array{3, 4}));
    static_assert(not std::ranges::ends_with(std::array{1, 2, 3, 4}, std::array{4, 5}));
}
}

namespace header_compare {
// https://en.cppreference.com/w/cpp/header/compare
void demo()
{
}
}


namespace adjacent_find {
// The std::adjacent_find searches for two consecutive elements that match the predicate.
// It returns an iterator to the first element of the match.
constexpr auto increasing = {1, 2, 3};
constexpr auto is_less = [](const auto& l, const auto& r) { return l > r; };
constexpr auto iter1 = std::ranges::adjacent_find(increasing, is_less);
static_assert(iter1 == increasing.end());
constexpr auto decreasing = {3, 2, 1};
constexpr auto iter2 = std::ranges::adjacent_find(decreasing, is_less);
static_assert(iter2 == decreasing.begin());
void demo() {}
}


namespace contains_and_contains_subrange {
void demo()
{
    constexpr auto haystack = std::array{3, 1, 4, 1, 5};
    constexpr auto needle = std::array{1, 4, 1};
    constexpr auto bodkin = std::array{2, 5, 2};
    static_assert(std::ranges::contains(haystack, 4));
    static_assert(not std::ranges::contains(haystack, 6));
    static_assert(std::ranges::contains_subrange(haystack, needle));
    static_assert(not std::ranges::contains_subrange(haystack, bodkin));

    constexpr std::array<std::complex<double>, 3> nums{
            {
                {1, 2},
                {3, 4},
                {5, 6}
            }
    };
    static_assert(std::ranges::contains(nums, std::complex<double>{3, 4}));
}
}


namespace regex {
void demo()
{
    std::string s =
        R"(Some people, when confronted with a problem, think "I'll use regular expressions." Now they have two problems.)";

    // Case-insensitive search for "regular expressions".
    std::regex self_regex("REGULAR EXPRESSIONS", std::regex_constants::ECMAScript | std::regex_constants::icase);
    assert(std::regex_search(s, self_regex));

    // Regex that finds words.
    std::regex word_regex(R"((\w+))");
    auto words_begin = std::sregex_iterator(s.begin(), s.end(), word_regex);
    auto words_end = std::sregex_iterator();

    // The number of words found.
    assert(std::distance(words_begin, words_end) == 18);

    // Words it found:
    // Some people when confronted with a problem think I ll use regular expressions Now they have two problems
    for (std::sregex_iterator i = words_begin; i != words_end; ++i)
    {
        std::smatch match = *i;
        [[maybe_unused]] std::string match_str = match.str();
    }

    std::regex long_word_regex(R"((\w{7,}))");
    std::string new_s = std::regex_replace(s, long_word_regex, "[$&]");
    assert(
        new_s ==
        R"(Some people, when [confronted] with a [problem], think "I'll use [regular] [expressions]." Now they have two [problems].)");

    // Regex matching.
    {
        const std::regex regex(R"(\w+@\w+\.(?:com))");
        assert(std::regex_match("good@mail.com", regex));
        assert(not std::regex_match("@bad.com", regex));
    }
}
}


namespace ranges_finding {
    void demo() {
        // Simple find integer value.
        {
            const auto numbers = std::list{2, 4, 3, 2, 3, 1};
            auto it = std::ranges::find(numbers, 2);
            assert(*it == 2);
        }

        // Find by specifying to search on a struct member.
        {
            struct Person
            {
                unsigned id;
                const char* name;
                const char* job;
            };
            std::list<Person> persons{
                {0, "Ana", "barber"},
                {1, "Bob", "cook"},
                {2, "Eve", "builder"}
            };
            auto it = std::ranges::find(persons, "Bob", &Person::name);
            assert(it->id == 1);
            assert(it->name == std::string("Bob"));
            assert(it->job == std::string("cook"));
        }

        // Demo of find on word length.
        {
            std::vector<std::string> names {"Apu", "Lisa", "Bart", "Ralph", "Homer", "Maggie"};
            auto iterator = std::ranges::find(names, 4, &std::string::size);
            assert(*iterator == "Lisa");
        }

        // Demo of finding duplicates in a range.
        {
            auto contains_duplicates_n2 = [](auto begin, auto end)
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
    }
}


namespace ranges_find_max {
void demo() {
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
            auto copy = std::vector<Student>{};
            std::ranges::copy_if(students, std::back_inserter(copy), by_year);
            auto it = std::ranges::max_element(copy, std::less{}, &Student::score);
            return it != copy.end() ? it->score : 0;
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
            const auto view1 = std::ranges::ref_view{s}; // Wrap container in a view.
            const auto view2 = std::ranges::filter_view{view1, by_year};
            auto view3 = std::ranges::transform_view{view2, &Student::score};
            auto it = std::ranges::max_element(view3);
            return it != view3.end() ? *it : 0;
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

}
}


namespace find_if {
// Demo of ranges::find_if
void demo() {
    const auto values = {4, 1, 3, 2};
    const auto is_even = [](int x) { return x % 2 == 0; };
    const auto iter = std::ranges::find_if(values, is_even);
    assert(*iter == 4);
}
}


void demo() {
    binary_search::demo();
    any_of_all_of_none_of::demo();
    starts_with_and_ends_with::demo();
    header_compare::demo();
    adjacent_find::demo();
    contains_and_contains_subrange::demo();
    regex::demo();
    ranges_finding::demo();
    ranges_find_max::demo();
    find_if::demo();
}
}
