#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>
#include "introduction.h"

#include <iomanip>

namespace ranges_library {

void introduction()
{
    // Conceptually a range is just a pair of two iterators.
    // One to the beginning and one to the end of a sequence.

    // Demonstration of how ranges makes code simpler.
    // First regular C++, then the same using ranges.
    {
        // Start from a list of numbers.
        const auto numbers = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        // Define a function to select even numbers.
        const auto even = [](int i) { return 0 == i % 2; };
        {
            // Regular C++ code example.
            // Copy the even numbers into a temporal container.
            std::vector<int> temp1;
            std::copy_if(begin(numbers), end(numbers), std::back_inserter(temp1), even);
            // Create a second temporal container which skips the first element.
            std::vector temp2(begin(temp1) + 1, end(temp1));
            // Print the result in reverse order.
            for (auto iter = std::rbegin(temp2); iter != std::rend(temp2); ++iter)
                std::cout << *iter << ' ';
            std::cout << std::endl;
        }
        {
            // Using ranges for simpler code.
            using namespace std::ranges::views;
            auto reverse_view = numbers | filter(even) | drop(1) | reverse;
            for (const int i : reverse_view)
                std::cout << i << ' ';
            std::cout << std::endl;
        }
    }
}
}
