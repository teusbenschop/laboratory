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

#include "atomics.h"

#include <atomic>
#include <cassert>
#include <future>
#include <iostream>
#include <thread>

namespace atomics {
namespace wait {
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


void demo() {
    wait::demo();
}
}
