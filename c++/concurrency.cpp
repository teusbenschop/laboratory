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
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <semaphore>
#include <thread>

namespace concurrency {
namespace atomic_wait {
// The std::atomic::wait performs atomic waiting operations.
// An old value is passed to the ::wait.
// It unblocks the thread if the atomic wait gets another value than the old value passed.
void demo()
{
    constexpr int task_count = 16;
    std::atomic<bool> all_tasks_complete{false};
    std::atomic<unsigned> completion_count{};
    std::future<void> futures[task_count];
    std::atomic<int> outstanding_task_count{task_count};

    // Spawn several tasks which take different amounts of time,
    // then decrement the outstanding task count.
    for (std::future<void>& task_future : futures)
        task_future = std::async([&]
        {
            // This sleep represents doing real work...
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(50ms);

            ++completion_count;
            --outstanding_task_count;

            // When the task count falls to zero, notify
            // the waiter (the main thread in this case).
            if (outstanding_task_count == 0)
            {
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


namespace async_and_future {
void demo()
{
    std::thread::id async_thread_id;
    const auto sum = [&](int a, int b) -> int
    {
        async_thread_id = std::this_thread::get_id();
        return a + b;
    };
    {
        std::thread::id main_thread_id = std::this_thread::get_id();
        std::future future = std::async(sum, 1, 2); // Call the function in a thread.
        const int result = future.get(); // Wait till the calculation is ready and get the result.
        assert(result == 3);
        assert(main_thread_id != async_thread_id);
    }
    {
        // Default launch policy (launch policy can be omitted).
        std::future future = std::async(std::launch::async | std::launch::deferred, sum, 1, 2);
        // std::launch::async: Run as soon as possible.
        // std::launch::deferred: Wait till result is requested, then run.
    }
}
}


namespace packaged_task {
// A std::packaged_task wraps any Callable target so that it can be invoked asynchronously.
// Its return value or exception thrown can be accessed through std::future objects.
void demo()
{
    // No need to pass a promise reference here.
    [[maybe_unused]] auto task_divide = [](int a, int b)
    {
        if (!b)
            throw std::runtime_error{"Divide by zero exception"};
        return a / b;
    };

    // std::packaged_task<decltype(task_divide)> task(task_divide);
    // auto future = task.get_future();
    // std::thread thread(std::move(task), 45, 5);
    // try {
    //     auto result = future.get();
    //     std::cout << result << std::endl;
    // } catch (const std::exception& exception) {
    //     std::cout << exception.what() << std::endl;
    // }
    // thread.join();
}
}


namespace semaphores {
void demo()
{
    return;
    // The counting_semaphore contains an internal counter initialized by the constructor.
    // This counter is decremented by calls to acquire() and related methods,
    // and is incremented by calls to release().
    // When the counter is zero, acquire() blocks until the counter is incremented.
    // The binary_semaphore is a counting_semaphore with max count = 1.
    std::binary_semaphore signal_main_to_thread_semaphore{0};
    std::binary_semaphore signal_thread_to_main_semaphore{0};

    const auto thread_processor = [&]()
    {
        // Wait for a signal from the main process by attempting to acquire (decrement) the semaphore.
        // This call blocks until the semaphore is released (its count is increased) from the main process.
        signal_main_to_thread_semaphore.acquire();
        std::cout << "Thread got the signal" << std::endl;

        // Wait shortly to imitate some work being done by the thread.
        using namespace std::literals;
        std::this_thread::sleep_for(10ms);

        // Signal the main process back.
        std::cout << "Thread sends the signal" << std::endl;
        signal_thread_to_main_semaphore.release();
    };

    // Create a worker thread
    std::jthread worker_thread(thread_processor);

    // Signal the worker thread to start working by releasing the semaphore (increasing its count).
    std::cout << "Main sends the signal" << std::endl;
    signal_main_to_thread_semaphore.release();

    // Wait until the worker thread is done doing the work
    // by attempting to decrement the semaphore's count.
    signal_thread_to_main_semaphore.acquire();

    std::cout << "Main got the signal" << std::endl;
}
}


namespace jthread {
// https://en.cppreference.com/w/cpp/thread/jthread
// It has the same general behavior as std::thread,
// except that jthread automatically joins on destruction,
// and can be canceled/stopped in certain situations.
void demo()
{
    return;
    {
        const auto worker = []
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::cout << "Within thread with ID " << std::this_thread::get_id() << std::endl;
        };
        auto jthread = std::jthread{worker};
        // The jthread will join automatically when it goes out of scope.
    }

    {
        using namespace std::literals::chrono_literals;
        std::jthread stoppable_thread{
            [](std::stop_token stoptoken)
            {
                while (!stoptoken.stop_requested())
                {
                    std::this_thread::sleep_for(1ms);
                }
            }
        };
        std::this_thread::sleep_for(25ms);
        stoppable_thread.request_stop();
        //  stoppable_thread.join(); // Not needed here.
    }

    {
        using namespace std::literals::chrono_literals;

        std::mutex mutex{};
        std::jthread threads[4];

        const auto print = [](const std::stop_source& source)
        {
            std::cout << std::boolalpha
                << "stop_source stop_possible = " << source.stop_possible() << std::endl
                << "stop_requested = " << source.stop_requested() << std::endl;
        };

        // Common stop source.
        std::stop_source stop_source;
        assert(stop_source.stop_requested() == false);
        assert(stop_source.stop_possible() == true);
        print(stop_source);

        const auto joinable_thread_worker = [&mutex](const int id, std::stop_source stop_source)
        {
            using namespace std::literals::chrono_literals;
            std::stop_token stoken = stop_source.get_token();
            while (true)
            {
                std::this_thread::sleep_for(3ms);
                std::lock_guard lock(mutex);
                if (stoken.stop_requested())
                {
                    std::cout << "worker " << id << " is requested to stop" << std::endl;
                    return;
                }
                std::cout << "worker " << id << " goes back to sleep" << std::endl;
            }
        };

        // Create worker threads.
        for (int i = 0; i < 4; ++i)
        {
            threads[i] = std::jthread(joinable_thread_worker, i + 1, stop_source);
        }

        std::this_thread::sleep_for(7ms);

        std::cout << "request stop" << std::endl;
        stop_source.request_stop();

        print(stop_source);
        // Note: destructor of jthreads will call join so no need for explicit calls
    }
}
}

namespace timer_with_jthread_and_timed_mutex_and_condition_variable {
void demo()
{
    return;
    std::timed_mutex mx;
    std::condition_variable_any cv; // The _any means: Works with any lock, not just a unique_lock.

    // This lambda function takes a stop token as parameters.
    const auto timer = [&](const std::stop_token& stoken)
    {
        std::cout << "Step 3: Set the timer interval to 100 milliseconds" << std::endl;
        constexpr auto interval = std::chrono::milliseconds(100);
        while (!stoken.stop_requested())
        {
            std::cout << "Step 4: The stop token has no stop request: Keep going" << std::endl;
            std::unique_lock ulk(mx);
            std::cout <<
                "Step 5: Enter the condition variable which will wait 100 ms or less in case of a thread stop request"
                << std::endl;
            if (cv.wait_for(ulk, stoken, interval, [&stoken] { return stoken.stop_requested(); }))
            {
                std::cout <<
                    "Step 8: The condition variable got a stop request and so interrupts its waiting state immediately"
                    << std::endl;
                break;
            }
            std::cout << "Step 6: Run one timer cycle" << std::endl;
        }
        std::cout << "Step 9: The thread function quits and the thread automatically joins" << std::endl;
    };

    std::cout << "Step 1: The main thread starts the timer thread" << std::endl;
    std::jthread thread(timer);
    std::cout << "Step 2: The main thread will sleep for 350 milliseconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
    std::cout << "Step 7: The jthread will go out of scope, this sends a stop request to the thread function" <<
        std::endl;
}
}



namespace condition_variables {
void demo()
{
    std::condition_variable cv;
    std::cv_status status;

    const auto timer = [&]()
    {
        std::mutex mx;
        std::unique_lock ulk(mx);
        status = cv.wait_for(ulk, std::chrono::microseconds(100));
    };
    {
        std::jthread thread1(timer);
    }
    // The condition variable ran into a timeout.
    assert(status == std::cv_status::timeout);
    // Or else: no_timeout.
}
}




void demo()
{
    atomic_wait::demo();
    timed_mutex::demo();
    async_and_future::demo();
    packaged_task::demo();
    semaphores::demo();
    jthread::demo();
    timer_with_jthread_and_timed_mutex_and_condition_variable::demo();
    condition_variables::demo();
}
}
