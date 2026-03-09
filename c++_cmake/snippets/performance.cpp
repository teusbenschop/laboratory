#include <vector>

#include "functions.h"
#include "scoped_timer.hpp"

namespace snippets {
void demo_performance_small_big_objects()
{
    // This example demonstrates that traversing a contiguous array
    // of small objects is faster than traversing an array of big objects.

    struct small_object
    {
        std::array<char, 4> data_{};
        int m_score{1};
    };

    struct big_object
    {
        std::array<char, 256> data_{};
        int m_score{1};
    };

    const auto sum_scores = [] (const auto& objects)
    {
        scoped_timer<std::chrono::microseconds> t{};
        auto sum = 0ul;
        for (const auto& obj : objects)
        {
            sum += static_cast<size_t>(obj.m_score);
        }
        return sum;
    };

    std::cout << "sizeof(small_object): " << sizeof(small_object) << " bytes" << std::endl;
    std::cout << "sizeof(big_object): " << sizeof(big_object) << " bytes" << std::endl;

    constexpr auto num_objects = 1'000'000ul;

    std::cout << "calculate using small_object" << std::endl;
    auto small_objects = std::vector<small_object>(num_objects);
    [[nodiscard]] const auto small_sum = sum_scores(small_objects);

    std::cout << "calculate using big_object" << std::endl;
    auto big_objects = std::vector<big_object>(num_objects);
    [[nodiscard]] const auto big_sum = sum_scores(big_objects);
}
}
