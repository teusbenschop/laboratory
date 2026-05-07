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
#include <barrier>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <random>
#include <semaphore>
#include <thread>
#include <queue>

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
            // This sleep represents doing real work.
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


namespace future_and_promise_and_exception {
// It is possible to set an exception in a promise.
// If it is set, then it gets thrown in the calling environment.
void demo()
{
    const auto divide = [] (int a, int b, std::promise<int>& promise) {
        try {
            if (not b)
                throw std::runtime_error("cannot divide by zero");
            const auto result = a / b;
            promise.set_value(result);
        } catch(...) {
            try {
                // Store the exception in the promise.
                promise.set_exception(std::current_exception());
                // Or store custom exception instead.
                // promise.set_exception(std::make_exception_ptr(MyException("mine")));
            } catch(...) {} // set_exception() may throw too.
        }
    };

    {
        std::promise<int> promise;
        std::jthread thread {divide, 45, 5, std::ref(promise)};
        auto future = promise.get_future();
        int result = future.get();
        assert (result == 9);
    }
    try {
        std::promise<int> promise;
        std::jthread thread {divide, 45, 0, std::ref(promise)};
        auto future = promise.get_future();
        auto result = future.get();
        assert(false);
    } catch (const std::exception& exception) {
        assert(exception.what() == std::string("cannot divide by zero"));
    }
}
}


namespace execution_policies {
void demo()
{
    std::array<int, 5> values = {1, 2, 3, 4, 5};
    [[maybe_unused]] const auto fn = []([[maybe_unused]] const int n)
    {
    };
    // std::for_each(std::execution::par_unseq, values.begin(), values.end(), fn);
    // Clang on macOS Tahoe does not support parallel execution.
    std::for_each(values.begin(), values.end(), fn);
}
}


namespace lock_multiple_simultaneously {
// This demonstrates how to use std::lock to lock multiple locks at once simultaneously.
// This avoids the risk of having deadlocks in the transfer function.
void demo()
{
    struct account {
        int m_balance{0};
        std::mutex m_mutex{};
    };

    const auto transfer_money = [](account& from, account& to, int amount) -> void {
        // Define two deferred unique locks.
        auto lock1 = std::unique_lock<std::mutex>{from.m_mutex, std::defer_lock};
        auto lock2 = std::unique_lock<std::mutex>{to.m_mutex, std::defer_lock};
        // Lock both unique_locks at the same time to avoid a deadlock.
        std::lock(lock1, lock2);
        // Do the transfer.
        from.m_balance -= amount;
        to.m_balance += amount;
        // End of scope releases locks.
    };

    auto account1 = account{100};
    auto account2 = account{30};
    transfer_money(account1, account2, 20);
    assert (account1.m_balance == 80);
    assert (account2.m_balance == 50);
}
}


namespace atomic_reference {
void demo()
{
    struct Statistics {
        int heads{};
        int tails{};
    } stats;

    const auto random_int = [](int min, int max) {
        // One engine instance per thread.
        static thread_local auto engine = std::default_random_engine{std::random_device{}()};
        auto distribution = std::uniform_int_distribution<>{min, max};
        return distribution(engine);
    };

    constexpr int flip_count {5000};

    const auto flip_coin = [&](std::size_t n, auto& outcomes) {
        auto flip = [&](auto n) {
            const auto heads = std::atomic_ref<int>{outcomes.heads};
            const auto tails = std::atomic_ref<int>{outcomes.tails};
            for (auto i = 0u; i < n; ++i)
                random_int(0, 1) == 0 ? ++heads : ++tails;
        };
        // Five threads for parallel flipping.
        std::jthread{flip, n / 5};
        std::jthread{flip, n / 5};
        std::jthread{flip, n / 5};
        std::jthread{flip, n / 5};
        std::jthread{flip, n / 5};
    };

    flip_coin(flip_count, stats); // Flip many times.
    assert(stats.heads + stats.tails == flip_count);
}
}


namespace barrier_jthread_stop_token {
void demo()
{
    return;
    // This lambda starts the failing processes.
    // On any failure, it restarts the processes.
    const auto resilient_processes = [](const std::stop_token& stop_token)
    {
        // Protect the barrier from being arrived at multiple times
        // due to multiple processes that may all fail simultaneously.
        std::atomic<bool> barrier_active{false};
        // As soon as the barrier is complete, clear the associated protecting flag.
        auto on_barrier_completion = [&]() noexcept
        {
            barrier_active = false;
        };
        // The barrier has count 2:
        // One to allow arriving and waiting at the barrier after all processes have been created.
        // The second one to use for the error callback.
        std::barrier barrier(2, on_barrier_completion);

        // Allow the main program to interrupt the processes loop.
        std::stop_callback stop_callback(stop_token, [&]()
        {
            if (barrier_active)
            {
                barrier_active = false;
                [[maybe_unused]] const auto token = barrier.arrive();
            }
        });

        bool slow_restart_down{false};

        while (!stop_token.stop_requested())
        {
            try
            {
                const auto on_event = [&]([[maybe_unused]] const std::string& error)
                {
                    // Arrive at the barrier.
                    // This will open the barrier, so all processes get recreated.
                    if (barrier_active)
                    {
                        barrier_active = false;
                        [[maybe_unused]] const auto token = barrier.arrive();
                        slow_restart_down = true;
                    }
                };

                // Wait shortly to avoid fast repeating error events.
                if (slow_restart_down)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                }

                // The processes start here.
                barrier_active = true;

                // Call the on_event callback to simulate an error.
                const auto generate_error = [&]()
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    on_event("Error");
                };
                std::thread(generate_error).detach(); // Better not detach in production code.

                // At this point all processes have started.

                // Wait here till an error occurs or the program shuts down.
                barrier.arrive_and_wait();
            }
            catch (const std::exception& ex)
            {
                std::cerr << ex.what() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    };

    std::jthread thread(resilient_processes);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // The thread goes out of scope here, so it gets the stop token.
}
}


namespace thread_pool {

class ThreadPool {
public:
    // Constructor to creates a thread pool with given number of threads.
    ThreadPool(size_t num_threads = std::thread::hardware_concurrency())
    {
        // Creating worker threads.
        for (size_t i = 0; i < num_threads; ++i) {
            m_threads.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    // The reason for putting the below code here is to unlock the queue
                    // before executing the task so that other threads can perform enqueue tasks.
                    {
                        // Locking the queue so that data can be shared safely.
                        std::unique_lock<std::mutex> lock(m_mutex);

                        // Waiting until there is a task to execute or the pool is stopped.
                        m_cv.wait(lock, [this] {
                            return !m_tasks.empty() or m_stop;
                        });

                        // Exit the thread in case the pool is stopped and there are no tasks.
                        if (m_stop && m_tasks.empty()) {
                            return;
                        }

                        // Get the next task from the queue
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    // Destructor to stop the thread pool.
    ~ThreadPool()
    {
        {
            // Lock the queue to update the stop flag safely.
            std::unique_lock<std::mutex> lock(m_mutex);
            m_stop = true;
        }

        // Notify all threads
        m_cv.notify_all();

        // Joining all worker threads to ensure they have completed their tasks.
        for (auto& thread : m_threads) {
            thread.join();
        }
    }

    // Enqueue task for execution by the thread pool
    void enqueue(std::function<void()> task)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_tasks.emplace(std::move(task));
        }
        m_cv.notify_one();
    }

private:
    // Vector to store worker threads
    std::vector<std::thread> m_threads;

    // Queue of tasks.
    std::queue<std::function<void()>> m_tasks;

    // Mutex to synchronize access to shared data.
    std::mutex m_mutex;

    // Condition variable to signal changes in the state of the tasks queue.
    std::condition_variable m_cv;

    // Flag to indicate whether the thread pool should stop or not.
    bool m_stop = false;
};


void demo()
{
    // Create a thread pool with 4 threads.
    ThreadPool pool(4);

    // Enqueue tasks for execution.
    for (int i = 0; i < 10; ++i) {
        pool.enqueue([i] {
            std::cout << "Task " << i << " is running on thread " << std::this_thread::get_id() << std::endl;
            // Simulate some work.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });
    }
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
    future_and_promise_and_exception::demo();
    execution_policies::demo();
    lock_multiple_simultaneously::demo();
    atomic_reference::demo();
    barrier_jthread_stop_token::demo();
    thread_pool::demo();
}
}
