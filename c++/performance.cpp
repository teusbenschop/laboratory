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

#include "performance.h"
#include "clocking.h"
#include <cassert>
#include <functional>
#include <iostream>
#include <latch>
#include <new>
#include <thread>
#include <vector>

namespace performance {


namespace hardware_interference {

// Minimum offset between two objects to avoid false sharing.
static_assert(std::hardware_destructive_interference_size == 64 or std::hardware_destructive_interference_size == 256);

// Maximum size of contiguous memory to promote true sharing.
static_assert(std::hardware_constructive_interference_size == 64);

void demo()
{
    return;
    std::vector<std::jthread> threads;
    const int hc = std::thread::hardware_concurrency();

    for (int number_of_threads = 1; number_of_threads <= hc; ++number_of_threads)
    {
        // Start all threads at the same time for exact time measurement.
        std::latch sync(number_of_threads);

        // Some individual piece of data for each individual thread.
        // struct { std::atomic<char> might_be_shared; } global_data[hc];

        // Mitigation: occupy a full cache line.
        struct alignas(std::hardware_destructive_interference_size) {
            std::atomic<char> might_be_shared;
        } global_data[hc];

        std::atomic<int64_t> total_threads_execution_time_ns(0);

        for (int t = 0; t != number_of_threads; ++t)
        {
            threads.emplace_back([&](int i)
            {
                // Synchronize thread execution start.
                sync.arrive_and_wait();

                const auto start = std::chrono::high_resolution_clock::now();

                for (std::size_t r = 10'000'00; r--;)
                    global_data[i].might_be_shared.fetch_add(1);

                total_threads_execution_time_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now() - start
                ).count();

            }, t);
        }

        // Join all threads.
        threads.clear();

        std::cout << "Number of threads " << number_of_threads
        << ": Thread execution time: "
        << total_threads_execution_time_ns / (1.0e6 * number_of_threads)
        << std::endl;
    }
}
}


namespace pointer_laundering {
// The std::launder prevents compiler from making false assumption about object lifetime and pointers.
// Usage: Old object is destroyed, create new same object in same memory.
// The old pointer cannot be used anymore.
// Pass the old pointer through std::launder makes compiler forget the old object and use the new one.

struct Struct
{
    const int value{};
};

void demo()
{
    // Allocate raw memory.
    alignas(Struct) unsigned char raw_memory[sizeof(Struct)];

    // Construct the first Struct in that raw memory.
    auto* ptr1 = new(raw_memory) Struct {10};
    assert(ptr1->value == 10);

    // Destroy the first Struct.
    ptr1->~Struct();

    // Reuse the same memory for a second Struct.
    auto* ptr2 = new(raw_memory) Struct {20};
    assert(ptr2->value == 20);

    // Using the original pointer here is undefined behavior, although it may work.
    assert(ptr1->value == 20);

    // "Launder" (wash) the old pointer to make it valid again.
    Struct* laundered_ptr = std::launder(ptr1);
    assert(laundered_ptr->value == 20);
}
}



namespace lambda_is_much_faster_than_bind {

int sum_fn(int a, int b, int c)
{
    return a + b + c;
}

void demo()
{
    return;
    int sum = 0;
    {
        const auto timer = scoped_timer::scoped_timer<std::chrono::microseconds>{};
        for (int a = 0; a < 100; ++a)
            for (int b = 0; b < 100; ++b)
                for (int c = 0; c < 100; ++c)
                {
                    const auto lambda_fn = [a, b, c]()
                    {
                        return sum_fn(a, b, c);
                    };
                    sum += lambda_fn();
                }
    }
    {
        const auto timer = scoped_timer::scoped_timer<std::chrono::microseconds>{};
        for (int a = 0; a < 100; ++a)
            for (int b = 0; b < 100; ++b)
                for (int c = 0; c < 100; ++c)
                {
                    const auto bind_fn = std::bind(sum_fn, a, b, c);
                    sum += bind_fn();
                }
    }
}
}


void demo()
{
    hardware_interference::demo();
    pointer_laundering::demo();
    lambda_is_much_faster_than_bind::demo();
}
}
