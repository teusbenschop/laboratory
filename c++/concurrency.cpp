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

#include "concurrency.h"

#include <atomic>
#include <cassert>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

namespace concurrency {

namespace atomic_wait {
// The std::atomic::wait performs atomic waiting operations.
// An old value is passed to the ::wait.
// It unblocks the thread if the atomic wait gets another value than the old value passed.
void demo() {
    constexpr int task_count = 16;
    std::atomic<bool> all_tasks_complete{false};
    std::atomic<unsigned> completion_count{};
    std::future<void> futures[task_count];
    std::atomic<int> outstanding_task_count{task_count};

    // Spawn several tasks which take different amounts of time,
    // then decrement the outstanding task count.
    for (std::future<void> &task_future: futures)
        task_future = std::async([&] {
            // This sleep represents doing real work...
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(50ms);

            ++completion_count;
            --outstanding_task_count;

            // When the task count falls to zero, notify
            // the waiter (the main thread in this case).
            if (outstanding_task_count == 0) {
                all_tasks_complete = true;
                all_tasks_complete.notify_one();
            }
        });

    // Wait here till the atomic variable has a value different from false.
    all_tasks_complete.wait(false);

    assert(completion_count.load() == task_count);
}
}


namespace timed_mutex {
// If a normal mutex cannot be obtained, this would lead to a deadlock.
// A timed mutex will assist in such a case.
// If a lock is requested on a timed mutex, a timeout can be passed too.
// If the lock cannot be obtained in time, it falls in a timeout, not in a deadlock.

std::timed_mutex timed_mutex;

void demo()
{
    return;
    std::cout << "Attempt to get first lock on timed mutex" << std::endl;
    const std::unique_lock lock1(timed_mutex, std::chrono::seconds(1));
    std::cout << "Got the first lock" << std::endl;
    std::cout << "Attempt to get the second lock on the same timed mutex" << std::endl;
    const std::unique_lock lock2(timed_mutex, std::chrono::milliseconds(10));
    if (lock2)
        std::cout << "Got second lock" << std::endl;
    else
        std::cout << "Failed to get second lock within 10 milliseconds" << std::endl;
}
}



void demo() {
    atomic_wait::demo();
    timed_mutex::demo();
}
}
