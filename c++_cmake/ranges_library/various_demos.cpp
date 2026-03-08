#include <algorithm>
#include <array>
#include <complex>
#include <iostream>
#include <list>
#include <numeric>
#include <random>
#include <vector>
#include "functions.h"

namespace ranges_library {
// Assisting functions for demonstration.

void print_range_values(auto&& r)
{
    std::ranges::for_each(r, [](auto&& i) { std::cout << i << ' '; });
    std::cout << std::endl;
}

void print_range_is_sorted(auto&& values)
{
    std::cout << (std::ranges::is_sorted(values) ? "sorted" : "unsorted") << std::endl;
}

// Demonstration of the ranges::contains(_subrange).
void demo_contains_subrange()
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
    static_assert(std::ranges::contains(nums, std::complex<double>{3, 4}));
}

// Demonstration of ranges sorting.
void demo_sorting()
{
    const auto values = std::vector{6, 3, 2, 7, 4, 1, 5};
    print_range_values(values);
    print_range_is_sorted(values);
    auto non_const_values = values;
    // Regular C++ sort: std::sort(non_const_values.begin(), non_const_values.end());
    std::ranges::sort(non_const_values);
    print_range_values(non_const_values);
    print_range_is_sorted(non_const_values);
}

// Demonstration of ranges transform.
void demo_transform()
{
    auto input = std::vector{1, 2, 3, 4};
    auto output = std::vector<int>(input.size());
    auto math_square = [](auto&& i) -> int { return i * i; };
    std::ranges::transform(input, output.begin(), math_square);
    std::cout << "std::ranges::transform: ";
    print_range_values(output);
}

// Demonstration of ranges fill.
void demo_fill()
{
    std::vector<int> v(5);
    std::ranges::fill(v, 123);
    std::cout << "std::ranges::fill: ";
    print_range_values(v);
}


// Demonstration of ranges generate.
void demo_generate()
{
    std::vector<int> v(4);
    std::ranges::generate(v, std::rand);
    std::cout << "std::ranges::generate: ";
    print_range_values(v);
}

// Demonstration of ranges iota.
void demo_iota()
{
    auto v = std::list<int>(6);
    // Fill the list with ascending values: 0, 1, 2, ...
    std::iota(v.begin(), v.end(), 0);
    std::cout << "std::(ranges::)iota: ";
    print_range_values(v);
    //std::ranges::iota(v, 0);

    // A vector of iterators.
    // Fill with iterators to consecutive list's elements
    std::vector<std::list<int>::iterator> vec(v.size());
    std::iota(vec.begin(), vec.end(), v.begin());
    //std::ranges::iota(vec.begin(), vec.end(), list.begin());
}

void demo_shuffle()
{
    std::cout << "std::ranges::shuffle: ";
    auto numbers = std::vector{1, 2, 3, 4, 5};
    print_range_values(numbers);
    std::ranges::shuffle(numbers, std::mt19937 {std::random_device {}()});
    print_range_values(numbers);
    print_range_is_sorted(numbers);
}


void demo_find()
{
    // Simple find integer value.
    {
        const auto numbers = std::list{2, 4, 3, 2, 3, 1};
        if (const auto it = std::ranges::find(numbers, 2);
            it != numbers.cend())
            std::cout << "std::ranges::find: " << *it << std::endl;
    }

    // Find by specifying to search on a struct member.
    {
        struct person {
            unsigned uid;
            const char* name;
            const char* position;
        };
        const auto persons = std::list<person> {
              {0, "Ana", "barber"},
              {1, "Bob", "cook"},
              {2, "Eve", "builder"}
        };
        if (const auto it = std::ranges::find(persons, "Bob", &person::name);
            it != persons.cend())
            std::cout << "std::ranges::find: " << it->uid << " " << it->name << " " << it->position << std::endl;
    }
}

void various_demos()
{
    demo_contains_subrange();
    demo_sorting();
    demo_transform();
    demo_fill();
    demo_generate();
    demo_iota();
    demo_shuffle();
    demo_find();
}

}
