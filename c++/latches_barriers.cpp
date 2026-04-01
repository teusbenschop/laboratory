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
#include <barrier>
#include <iostream>
#include <latch>
#include <random>
#include <thread>
#include <vector>

#include "latches_barriers.h"

// Latches and barriers are thread coordination mechanisms
// that allow any number of threads to block until an expected number of threads arrive.
// A latch cannot be reused, while a barrier can be used repeatedly.


namespace latches {

// The std::latch is a downward counter which can be used to synchronize threads.
// The value of the counter is initialized on creation.
// Threads may block on the latch until the counter is decremented to zero.
// There is no possibility to increase or reset the counter, the latch is single-use.

// The std::barrier is similar to the std::latch with these differences:
// 1. It can be reused.
// 2. It executes possibly empty callable before unblocking threads.

constexpr auto n_threads = 3;

void demo()
{
    return;
    // The std::latch starts with a given count.
    auto latch = std::latch{n_threads};

    std::vector<std::thread> threads;

    for (auto i = 0; i < n_threads; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        threads.emplace_back([&]
        {
            std::cout << "Thread " << std::this_thread::get_id() << " waits at latch and decreases it" << std::endl;
            latch.arrive_and_wait(); // Or just count_down();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::cout << "Thread " << std::this_thread::get_id() << " gets past latch" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
    }

    std::cout << "Main thread waits at latch till threads have counted it down to zero" << std::endl;
    latch.wait();
    std::cout << "Main thread get past the latch" << std::endl;
    for (auto&& thread : threads)
        thread.join();
    std::cout << "All threads have completed" << std::endl;
}



}


namespace barriers {


void demo()
{
    return;
    auto done{false};
    constexpr auto n_dice = 5;
    auto dice = std::array<int, n_dice>{};
    auto threads = std::vector<std::thread>{};
    auto n_turns{0};

    const auto get_random_int = [](const int min, const int max) -> int
    {
        // One engine instance per thread.
        thread_local auto engine = std::default_random_engine{std::random_device{}()};
        auto distribution = std::uniform_int_distribution{min, max};
        return distribution(engine);
    };

    // A function to run on completion of a barrier.
    // It checks whether all dice have rolled to six simultaneously.
    auto on_barrier_completion = [&] -> void
    {
        ++n_turns;
        const auto is_six = [](auto i) { return i == 6; };
        done = std::ranges::all_of(dice, is_six);
    };

    // Define the barrier to use with the completion callback.
    // After the barrier has counted down to zero,
    // and after the completion callback has run,
    // the barrier resets itself to its initial state.
    // Then the barrier can be used again.
    auto barrier = std::barrier{n_dice, on_barrier_completion};
    for (size_t i = 0; i < n_dice; i++)
    {
        threads.emplace_back([&, i]
        {
            while (!done)
            {
                // Roll dice.
                dice[i] = get_random_int(1, 6);
                // Decrement the barrier count by 1, wait here till the barrier is 0,
                // and then until the phase completion step of the current phase is run.
                barrier.arrive_and_wait();
            }
        });
    }
    for (auto&& t : threads)
    {
        t.join();
    }
    std::cout << "Number of turns to have all dice on 6: " << n_turns << std::endl;
}
}