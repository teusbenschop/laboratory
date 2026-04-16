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

#include "shared_mutex.h"

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <shared_mutex>
#include <thread>

namespace shared_mutex {
    // This example code features a queue.
    // Some code writes to it, and some code reads from it.
    // To synchronize the read and write operations, the following locks are used:
    // 1. A shared lock to enable one or more processes to read from the queue simultaneously.
    // 2. A unique lock so only one thread can write to the queue at any time.

    std::queue<int> the_queue;
    std::shared_mutex the_shared_mutex;
    std::condition_variable_any the_cv;

    void consume_queue(const std::stop_token &stoken) {
        while (!stoken.stop_requested()) {
            // Wait for the condition variable to be signaled, or for a stop request.
            // This uses a unique lock because it might modify the queue.
            std::unique_lock lock(the_shared_mutex);
            the_cv.wait_for(lock, stoken, std::chrono::seconds(1), [&stoken] {
                return stoken.stop_requested() or not the_queue.empty();
            });

            // If a stop is requested, do that right now.
            if (stoken.stop_requested())
                break;

            // Print / remove the value at the front of the queue.
            std::cout << "Removing " << the_queue.front() << " from queue" << std::endl;
            the_queue.pop();
        }
    }


    bool queue_empty() {
        // This uses a shared lock because it does not modify the queue.
        std::shared_lock lock(the_shared_mutex);
        return the_queue.empty();
    }

    void demo() {

        return;

        std::jthread consumer(consume_queue);

        for (int i{1}; i <= 5; i++) {
            // This uses a unique lock because it modifies the queue.
            std::cout << "Attempt to obtain a unique lock" << std::endl;
            std::unique_lock lock(the_shared_mutex);
            std::cout << "The unique lock was obtained" << std::endl;
            std::cout << "Push " << i << " onto queue" << std::endl;
            the_queue.push(i);
            // Signal the condition variable.
            the_cv.notify_one();
            // Wait shortly.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // Wait till the queue is empty.
        while (!queue_empty())
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
