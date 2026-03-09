#include <array>
#include <iostream>
#include <random>

#include "functions.h"

namespace numerics_library {

void pseudo_random_number_generation()
{
    {
        // Poisson distribution.
        // The probability of an event happening a certain number of times (k)
        // within a given interval of time.
        // Construct it around a given mean event count.
        constexpr const float mean_event_count{4.5};
        std::poisson_distribution<int> distribution(mean_event_count);
        std::default_random_engine generator;

        // Store how often an event occurs.
        constexpr const int array_size {10};
        std::array<int,array_size> events {};

        constexpr const int number_of_experiments {1000};
        constexpr const int number_of_stars {100};

        for (int i = 0; i < number_of_experiments; ++i) {
            const int number = distribution(generator);
            if (number < array_size) events[number]++;
        }

        std::cout << "Poisson distribution around mean event count " << mean_event_count << std::endl;
        for (int i = 0; i < array_size; ++i)
            std::cout << i << ": " << std::string(events[i] * number_of_stars / number_of_experiments, '*') << std::endl;
    }

    // Simulate throwing a 6-sided dice.
    {
        // A seed source for the random number engine.
        std::random_device rd;
        // A Mersenne Twister_random engine seeded with the random device.
        std::mt19937 mt_generator(rd());
        // A uniform distribution between 1 and 6 inclusive.
        std::uniform_int_distribution<> uniform_distribution(1, 6);

        // Throw the dice multiple times.
        std::cout << "Normal distribution for throwing a dice" << std::endl;
        for (int n = 0; n < 20; n++)
            std::cout << uniform_distribution(mt_generator) << " ";
        std::cout << std::endl;
    }
}

}