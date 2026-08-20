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
#include <charconv>
#include <iostream>
#include <ostream>
#include <random>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>
#include "transformations.h"

namespace transformations {

namespace accumulate {
constexpr std::array integers{1, 2, 3};
constexpr auto sum = std::accumulate(integers.cbegin(), integers.cend(), 0); // Default = sum.
static_assert(sum == 6);
constexpr auto product = std::accumulate(integers.cbegin(), integers.cend(), 1, std::multiplies());
static_assert(product == 6);

static void demo()
{
    const auto strings = std::vector<std::string>{"a", "b"};
    const std::string init{"init"};
    const auto concat = std::accumulate(strings.cbegin(), strings.cend(), init);
    assert(concat == "initab");

    const auto dash_fold = [](std::string a, int b)
    {
        return std::move(a) + '-' + std::to_string(b);
    };
    const std::string s = std::accumulate(std::next(integers.begin()), integers.end(),
                                    std::to_string(integers.at(0)), // start with first element
                                    dash_fold);
    assert(s == "1-2-3");
}
}


namespace copying {
static void demo()
{
    {
        constexpr auto values = std::array{1, 2, 3};
        std::vector<int> copy{};
        std::ranges::copy(values, std::back_inserter(copy));
        assert(copy.size() == 3);
    }
    {
        constexpr auto values = std::array{1, 1, 2, 2, 3, 3, 2, 2, 1, 1};
        std::vector<int> unique_copy{};
        std::ranges::unique_copy(values, std::back_inserter(unique_copy));
        // Copies values skipping consecutive equal elements.
        decltype(unique_copy) standard {1, 2, 3, 2, 1};
        assert(unique_copy == standard);
    }
}
}


namespace ranges_views_filter_drop_reverse {
static void demo()
{
    // Start from a list of numbers.
    const auto numbers = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    // Define a function to select even numbers.
    const auto even = [](const int i) -> bool
    {
        return i % 2 == 0;
    };
    // Define a view that filters on even numbers, then drops the first number, then reverses them.
    using namespace std::ranges::views;
    auto&& result = numbers | filter(even) | drop(1) | reverse;
    // The view contains: 8 6 4 2
    for (const int i : result)
        assert(i == 8 or i == 6 or i == 4 or i == 2);
}
}


namespace ranges_transformations {
static void demo() {
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
        std::ranges::for_each(squared_view, [&sum](int i) {sum += i;});
        // The transformed output will be: 1 4 9 16 25 36 49 64 81 100
        assert(sum == 385);
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
        assert(not std::ranges::is_sorted(numbers ));
    }

    // Demo of drop_while.
    {
        std::vector vec{1, 2, 3, 4, 5, 4, 3, 2, 1};
        // Apply the range adaptor that represents view of elements from an underlying sequence,
        // beginning at the first element for which the predicate returns false.
        auto v = vec | std::views::drop_while([](auto i) { return i < 5; });
        // Result: 5 4 3 2 1
        auto result = v | std::ranges::to<std::vector<int>>();
        auto standard = std::vector<int>{5, 4, 3, 2, 1};
        assert(result == standard);
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

    // Demo of join.
    {
        const auto list_of_lists = std::vector<std::vector<int>>{{1, 2}, {3, 4, 5}, {5}, {4, 3, 2, 1}};
        const auto flattened_view = std::views::join(list_of_lists);
        std::vector<int> result = flattened_view | std::ranges::to<std::vector<int>>();
        // Result: 1 2 3 4 5 5 4 3 2 1
        const auto standard = std::vector{1, 2, 3, 4, 5, 5, 4, 3, 2, 1};
        assert(result == standard);
    }

    // Demo of element<n>.
    {
        const std::vector<std::tuple<int, int>> vec { {  1, 2 }, {  3, 4 } };
        auto&& range = vec | std::views::elements<0>;
        for (const int i : range)
            assert(i == 1 or i == 3);
    }
}
}


namespace ranges_sorting {
static void demo()
{
    {
        // Standard sorting demo.
        std::vector values {6, 3, 2, 7, 4, 1, 5};
        assert(not std::ranges::is_sorted(values));
        // Regular C++ sort.
        // ReSharper disable once CppUseRangeAlgorithm
        std::sort(values.begin(), values.end());
        // Ranges sort.
        std::ranges::sort(values);
        const auto standard = std::vector{1, 2, 3, 4, 5, 6, 7};
        assert(values == standard);
        assert(std::ranges::is_sorted(values));
    }
    {
        // Demo of sorting on property, in this case, word length.
        std::vector<std::string> names {"Ralph", "Lisa", "Homer", "Maggie", "Apu", "Bart"};
        std::ranges::sort(names, std::less{}, &std::string::size);
        // The names are now sorted on length.
        const std::vector<std::string> standard {"Apu", "Lisa", "Bart", "Ralph", "Homer", "Maggie"};
        assert(names == standard);
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
        // Sort players by level, then by health.
        std::ranges::sort(players, std::less<>{}, level_and_health);
        assert(players.at(0).name == "Aki");
        assert(players.at(1).name == "Rei");
        assert(players.at(2).name == "Nao");
    }
    {
        // The partial_sort takes input range (begin, end), and iterator (middle).
        // It finds the smallest n elements in the entire collection
        // and places them, sorted, in the range from first up to middle.
        // The remaining elements from middle to last are left in an unspecified, unsorted order.
        std::vector values {6, 3, 2, 7, 4, 1, 5};
        std::ranges::partial_sort(values, values.begin() + 3);
        const auto standard = std::vector{1, 2, 3, 7, 6, 4, 5};
        assert(values == standard);
    }
    {
        // The nth_element takes a range as input, and an iterator pointing to the nth element,
        // and makes the range begin-nth to be less than the range nth-end.
        // In short it puts one specific element in its correct sorted position,
        // while partitioning the rest.
        std::vector values {6, 3, 2, 7, 4, 1, 5};
        std::ranges::nth_element(values, values.begin() + 1);
        const auto standard = std::vector{1, 2, 3, 4, 5, 6, 7};
        assert(values == standard);
        // This implementation sorts everything (which is not forbidden by the standard).
    }
    {
        // The stable_sort sorts the range and preserves the order of equivalent elements.
        struct People
        {
            std::string name{};
            int age{};
        };
        std::vector<People> people{
            {.name = "Name1", .age = 2},
            {.name = "Name1", .age = 1},
            {.name = "Name2", .age = 2},
            {.name = "Name3", .age = 3}
        };
        std::ranges::stable_sort(people, {}, &People::name);
        assert(people.at(0).age == 2);
        assert(people.at(1).age == 1);
        std::ranges::stable_sort(people, {}, &People::age);
        assert(people.at(0).name == "Name1");
        assert(people.at(1).name == "Name1");
        assert(people.at(2).name == "Name2");
    }
    {
        // The stable_partition puts items matching the predicate at the front,
        // returns an iterator indicating the remainder of items till the end.
        // It preserves the original order of elements.
        // The plain "partition" does not preserve the order.
        std::vector v {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto tail = std::ranges::stable_partition(v, [](const int i) { return i%2 == 0;});
        std::vector standard {
            0, 2, 4, 6, 8, // Partitioned.
            1, 3, 5, 7, 9 // Remainder.
        };
        assert(v == standard);
        assert(std::distance(v.begin(), tail.begin()) == 5);
    }
}
}


namespace charconv {
// Functions are ultra-fast, basic, non-throwing, non-allocating.
static void demo()
{
    {
        constexpr auto str {"1234"};
        int result{};
        const auto [ptr, ec] = std::from_chars(str, str + strlen(str), result);
        assert(ec == std::errc()); // No error.
        assert(result == 1234);
    }
    {
        constexpr auto str {"123 foo"};
        int result{};
        const auto [ptr, ec] = std::from_chars(str, str + strlen(str), result);
        assert(ec == std::errc()); // No error.
        assert(result == 123);
    }
    {
        constexpr auto str {"bar"};
        int result{};
        const auto [ptr, ec] = std::from_chars(str, str + strlen(str), result);
        assert(ec == std::errc::invalid_argument); // Not a number.
        assert(result == 0);
    }
    {
        constexpr auto str {"500000000000"};
        int result{};
        const auto [ptr, ec] = std::from_chars(str, str + strlen(str), result);
        assert(ec == std::errc::result_out_of_range); // Number would have been too large.
        assert(result == 0);
    }
    {
        constexpr std::size_t size = 2;
        char buf[size]{};
        const std::to_chars_result result = std::to_chars(buf, buf + size, 42);
        assert(result.ec == std::errc());
        const std::string_view str(buf, result.ptr - buf);
        assert(str == "42");
    }
    {
        constexpr std::size_t size = 10;
        char buf[size]{};
        const std::to_chars_result result = std::to_chars(buf, buf + size, 3.1415926535, std::chars_format::fixed, 10);
        assert(result.ec == std::errc::value_too_large);
    }
}
}


namespace tuple_apply {
// Run a function on a tuple and pass the tuple elements as arguments to the function.

static int add_function (const int a, const int b) { return a + b; };

template <typename T>
static T add_generic (const T a, const T b) { return a + b; };

static auto add_lambda = [] (const auto a, const auto b) { return a + b; };

template <typename... Ts>
[[maybe_unused]] static std::ostream& operator<< (std::ostream& os, const std::tuple<Ts...>& tuple)
{
    std::apply(
        [&os] (const Ts&... args)
        {
            os << "[";
            std::size_t n{0};
            ((os << args << (++n != sizeof...(Ts) ? ", " : "")), ...);
            os << "]";
        }, tuple
    );
    return os;
}

static void demo()
{
    {
        const int i = std::apply(add_function, std::pair{1, 2});
        assert(i == 3);
    }
    {
        const int i = std::apply(add_generic<int>, std::pair{1, 2});
        assert(i == 3);
    }
    {
        constexpr int i = std::apply(add_lambda, std::pair{1, 2});
        assert(i == 3);
    }
    {
        std::ostringstream oss;
        oss << std::tuple {123, "hello", 3.3f, 't'};
        assert(oss.str() == "[123, hello, 3.3, t]");
    }
}
}


namespace make_from_tuple {
static void demo()
{
    struct Struct
    {
        Struct(const int i, const float f, const char c) : i(i), f(f), c(c) {};
        int i{};
        float f{};
        char c{};
    };

    constexpr auto tuple = std::tuple<int, float, char>{1, 2.0f, 'c'};
    const Struct strct = std::make_from_tuple<Struct>(std::move(tuple));
    assert(strct.i == 1);
    assert(strct.f == 2.0f);
    assert(strct.c == 'c');
}
}


namespace scanning {
static void demo()
{
    {
        // Default binary operator is summing.
        // Outputs init(=0) -> 0+1(=1) -> 1+2(=3), excludes last element of input.
        // Named "exclusive" because the n-th element of the input
        // is "excluded" from the calculation of the n-th output element.
        const std::vector data{1, 2, 3, 4};
        std::vector<int> out;
        std::exclusive_scan(data.begin(), data.end(), std::back_inserter(out), 0);
        const std::vector standard {0, 1, 3, 6};
        assert(out == standard);
    }
    {
        // Outputs 1, 1+2=3, 1+2+3=6, includes last element.
        // The calculated n-th output "includes" the n-th input element.
        const std::vector data{1, 2, 3};
        std::vector<int> out;
        std::inclusive_scan(data.begin(), data.end(), std::back_inserter(out));
        const std::vector standard {1, 3, 6};
        assert(out == standard);
    }
    {
        const std::vector data{1, 2, 3, 4};
        std::vector<int> out;
        std::inclusive_scan(data.begin(), data.end(), std::back_inserter(out), std::multiplies<int>{});
        const std::vector standard {1, 2, 6, 24};
        assert(out == standard);
    }
}
}


namespace reduce {

// The std::reduce returns the generalized sum of "init" and the given range, over, by default, operator +,
// or another operator.
// Behaves like std::accumulate, except it may arbitrarily rearrange and regroup the elements.

static void demo()
{
    const std::vector data{1, 2, 3};

    // Sum of the input data.
    const int sum1 = std::reduce(data.cbegin(), data.cend());
    assert(sum1 == 6);

    // Take initial value, sum with input data.
    constexpr int init2 {2};
    const int sum2 = std::reduce(data.cbegin(), data.cend(), init2);
    assert(sum2 == 8);

    // Same as above, but with product.
    constexpr int init3 {3};
    const int sum3 = std::reduce(data.cbegin(), data.cend(), init3, std::multiplies<>());
    // 3 x 1 x 2 x 3 = 18.
    assert(sum3 == 18);
}
}


namespace remove_erase {
static void demo()
{
    std::string s {"A B C D"};
    // Removing means: Shifting elements to be kept to the front through move assignment.
    const auto ret = std::ranges::remove(s, ' '); // Remove all spaces.
    // Returns range right past the range to be kept.
    // The input container, a string, remains of same size.
    assert (s == "ABCDC D");
    // Actually remove: Remove the part from the iterator to the end of the string.
    s.erase(ret.begin(), ret.end());
    assert (s == "ABCD");
}
}


namespace set_union_difference_intersection {
static void demo()
{
    std::vector v1 {1, 2, 3};
    std::vector v2 {2, 3, 4};
    decltype(v1) output;
    std::ranges::set_union(v1, v2, std::back_inserter(output));
    decltype(v1) standard {1, 2, 3, 4};
    assert(output == standard);

    // The set_difference copies the elements from the sorted input1 range,
    // which are not found in the sorted input2 range,
    // to the output range.
    output.clear();
    std::ranges::set_difference(v1, v2, std::back_inserter(output));
    standard = {1};
    assert(output == standard);

    // The set_intersection constructs a sorted range with elements found in both sorted input ranges.
    output.clear();
    std::ranges::set_intersection(v1, v2, std::back_inserter(output));
    standard = {2, 3};
    assert(output == standard);
}
}


void demo()
{
    accumulate::demo();
    copying::demo();
    ranges_views_filter_drop_reverse::demo();
    ranges_transformations::demo();
    ranges_sorting::demo();
    charconv::demo();
    tuple_apply::demo();
    make_from_tuple::demo();
    scanning::demo();
    reduce::demo();
    remove_erase::demo();
    set_union_difference_intersection::demo();
}
}
