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
#include <list>
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
static void demo()
{
    constexpr int task_count {16};
    std::atomic all_tasks_complete{false};
    std::atomic<unsigned> completion_count{};
    std::future<void> futures[task_count];
    std::atomic outstanding_task_count{task_count};

    // Spawn several tasks that do some work, then update the global state.
    std::ranges::for_each(futures, [&](std::future<void>& future)
    {
        future = std::async([&]
        {
            // This sleep represents doing real work.
            using namespace std::literals;
            std::this_thread::sleep_for(50ms);

            // Update global state.
            ++completion_count;
            --outstanding_task_count;

            // When the task count gets zero, notify the waiter (the main thread in this case).
            if (not outstanding_task_count)
            {
                all_tasks_complete = true;
                all_tasks_complete.notify_one();
            }
        });
    });

    // Wait here till the atomic variable gets notified and has a value different from false.
    all_tasks_complete.wait(false);

    assert(completion_count == task_count);
}
}


namespace atomic {
// https://cppreference.com/cpp/atomic/atomic

// If one thread writes to an atomic object while another thread reads from it,
// the behavior is well-defined (see memory model for details on data races).

static void demo()
{
    std::atomic<int> atomic {};
    constexpr int standard {10};
    const auto reset_value = [&atomic] { atomic = standard; };

    // .store: atomically replace the value of the atomic.
    reset_value();
    atomic.store(standard + 1);
    assert(atomic == standard + 1);

    // .load: atomically obtain the value of the atomic.
    reset_value();
    assert(atomic.load() == standard);

    // .exchange: atomically replace the value of the atomic and return the value held previously.
    reset_value();
    assert(atomic.exchange(standard + 1) == standard);
    assert(atomic == standard + 1);

    // .compare_exchange_strong (_weak):
    // atomically compare the value of the atomic object with that of the expected argument.
    // 1. If equal:   perform atomic exchange: Put "desired" into atomic object.
    // 2. If unequal: perform atomic load: Put atomic object in "expected".
    // Parameter expected: reference to the value expected to be found in the atomic object.
    // Parameter desired: the value to store in the atomic object if it is as expected.
    // Returns: true/false whether the atomic value was changed.
    {
        reset_value();
        int expected = standard - 1;
        constexpr int desired = standard + 1;
        const bool exchanged = atomic.compare_exchange_strong(expected, desired);
        assert(not exchanged);
        assert(expected == standard);
        assert(desired == standard + 1);
        assert(atomic == standard);
    }

    // .fetch_add: atomically add the argument to the atomic and return previous value of atomic.
    reset_value();
    assert(atomic.fetch_add(5) == standard);
    assert(atomic == standard + 5);

    // .fetch_sub: atomically subtract the argument from the atomic and return previous value of atomic.
    reset_value();
    assert(atomic.fetch_sub(5) == standard);
    assert(atomic == standard - 5);

    // .fetch_and
    // .fetch_or
    // .fetch_xor
}
}


namespace timed_mutex {
// If a normal mutex cannot be obtained, this would lead to a deadlock.
// A timed mutex will help here.
// If a lock is requested on a timed mutex, a timeout can be passed too.
// If the lock cannot be obtained in time, it falls in a timeout, not in a deadlock.

static std::timed_mutex timed_mutex;

static void demo()
{
    // Attempt to get first lock on timed mutex.
    const std::unique_lock lock1(timed_mutex, std::chrono::milliseconds(10));
    // Attempt to get the second lock on the same timed mutex.
    const std::unique_lock lock2(timed_mutex, std::chrono::milliseconds(10));
    // Test locks status.
    assert(    lock1.owns_lock());
    assert(not lock2.owns_lock());
}
}


namespace async_and_future {
static void demo()
{
    std::thread::id async_thread_id;
    const auto sum = [&](const int a, const int b) -> int
    {
        async_thread_id = std::this_thread::get_id();
        return a + b;
    };
    {
        const std::thread::id main_thread_id = std::this_thread::get_id();
        std::future future = std::async(sum, 1, 2); // Call the function in a thread.
        const int result = future.get(); // Wait till the calculation is ready and get the result.
        assert(result == 3);
        assert(main_thread_id != async_thread_id);
    }
    {
        // Default launch policy (launch policy can be omitted).
        [[maybe_unused]] std::future future = std::async(std::launch::async | std::launch::deferred, sum, 1, 2);
        // std::launch::async: Run as soon as possible.
        // std::launch::deferred: Wait till result is requested, then run.
    }
}
}


namespace packaged_task {
// A std::packaged_task wraps any callable target so that it can be invoked asynchronously.
// Its return value or exception thrown can be accessed through the std::future object.
static void demo()
{
    // No need to pass a promise reference here.
    const auto task_divide = [](const int a, const int b)
    {
        if (not b)
            throw std::runtime_error{"Divide by zero exception"};
        return a / b;
    };

    // ReSharper disable once CppTemplateArgumentsCanBeDeduced
    std::packaged_task<int(int,int)> task (task_divide);
    std::future future = task.get_future();
    std::thread thread(std::move(task), 45, 5);
    try {
        const int result = future.get();
        assert(result == 9);
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
    }
    thread.join();
}
}


namespace semaphores {
static void demo()
{
    // The counting_semaphore contains an internal counter initialized by the constructor.
    // The template parameter gives the maximum value for this counter.
    // This counter is decremented by calls to acquire() and related methods,
    // and is incremented by calls to release().
    // Acquiring looks whether it still has a number > 0 that can be acquired.
    // Releasing returns number 1 (by adding) to the semaphore.
    // When the counter is zero, acquire() blocks until the counter is incremented.
    // The binary_semaphore is a counting_semaphore with max count = 1.
    std::binary_semaphore signal_main_to_thread_semaphore{0};
    std::binary_semaphore signal_thread_to_main_semaphore{0};

    const auto thread_processor = [&]
    {
        // Wait for a signal from the main process by attempting to acquire (decrement) the semaphore.
        // This call blocks until the semaphore is released (its count is increased) from the main process.
        signal_main_to_thread_semaphore.acquire();
        // Thread got the signal now.

        // Do some work in the thread.
        using namespace std::literals;
        std::this_thread::sleep_for(10ms);

        // Signal the main process back.
        signal_thread_to_main_semaphore.release();
    };

    // Create a worker thread
    std::jthread worker_thread(thread_processor);

    // Signal the worker thread to start working by releasing the semaphore (increasing its count).
    signal_main_to_thread_semaphore.release();

    // Wait until the worker thread is done doing the work
    // by attempting to decrement the semaphore's count.
    signal_thread_to_main_semaphore.acquire();
    // Main got the signal from the thread.
}
}


namespace jthread {
// https://en.cppreference.com/w/cpp/thread/jthread
// It has the same general behavior as std::thread,
// except that jthread automatically joins on destruction,
// and can be canceled/stopped in certain situations.
static void demo()
{
    {
        const auto worker = []
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        };
        std::jthread{worker};
        // The jthread will join automatically when it goes out of scope.
    }

    {
        using namespace std::literals;
        std::jthread jt{
            [](const std::stop_token& stop_token)
            {
                while (!stop_token.stop_requested())
                {
                    std::this_thread::sleep_for(1ms);
                }
            }
        };
        std::this_thread::sleep_for(25ms);
        jt.request_stop();
        //jt.join(); // Not needed here.
    }

    {
        using namespace std::literals;

        std::mutex mutex{};

        // Common stop source.
        std::stop_source stop_source;
        assert(stop_source.stop_requested() == false);
        assert(stop_source.stop_possible() == true);

        const auto joinable_thread_worker = [&mutex](const std::stop_source& stop_source)
        {
            const std::stop_token stop_token = stop_source.get_token();
            while (true)
            {
                std::this_thread::sleep_for(3ms);
                std::lock_guard lock(mutex);
                if (stop_token.stop_requested())
                    return;
            }
        };

        // Create worker threads.
        // ReSharper disable once CppTooWideScopeInitStatement
        std::jthread threads[4];
        for (auto & thread : threads)
        {
            thread = std::jthread(joinable_thread_worker, stop_source);
        }

        std::this_thread::sleep_for(7ms);

        // Request a stop once, it will propagate to all derived stop tokens.
        stop_source.request_stop();

        // Note: destructor of jthread will call .join() so no need for explicit calls.
    }
}
}

namespace condition_variables {
static void demo()
{
    const auto timer = []
    {
        std::condition_variable cv;
        std::mutex mx;
        std::unique_lock ulk(mx);
        const std::cv_status status = cv.wait_for(ulk, std::chrono::microseconds(100));
        // The condition variable ran into a timeout.
        assert(status == std::cv_status::timeout);
        // Or else: no_timeout.
    };
    std::jthread thread(timer);
}
}


namespace future_and_promise_and_exception {
// It is possible to set an exception in a promise.
// If it is set, then it gets thrown on future::get() in the calling environment.
static void demo()
{
    const auto divide = [] (const int a, const int b, std::promise<int>& promise) {
        try {
            if (not b)
                throw std::range_error("");
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
        const int result = future.get();
        assert (result == 9);
    }
    try {
        std::promise<int> promise;
        std::jthread thread {divide, 45, 0, std::ref(promise)};
        auto future = promise.get_future();
        [[maybe_unused]] auto result = future.get(); // It will throw on .get().
        assert(false);
    }
    catch (std::range_error& e) {
        assert(true);
    }
    catch (...) {
        assert(false);
    }
}
}


namespace execution_policies {
static void demo()
{
    std::array values = {1, 2, 3, 4, 5};
    const auto fn = [](const int){ };
    //std::for_each(std::execution::par_unseq, values.begin(), values.end(), fn);
    // Clang on macOS Tahoe does not support parallel execution.
    std::for_each(values.begin(), values.end(), fn);
}
}


namespace lock_multiple_simultaneously {
// This demonstrates how to use std::lock to lock multiple locks simultaneously.
// This avoids the risk of having deadlocks in the transfer function.
static void demo()
{
    struct account {
        int m_balance{0};
        std::mutex m_mutex{};
    };

    const auto transfer_money = [](account& from, account& to, const int amount) {
        // Define two deferred unique locks.
        auto ul1 = std::unique_lock<std::mutex>{from.m_mutex, std::defer_lock};
        auto ul2 = std::unique_lock<std::mutex>{to.m_mutex, std::defer_lock};
        // Lock both at the same time to avoid a deadlock.
        std::lock(ul1, ul2);
        // Do the transfer.
        from.m_balance -= amount;
        to.m_balance += amount;
        // End of scope releases locks.
    };

    auto account_one = account{100};
    auto account_two = account{30};
    transfer_money(account_one, account_two, 20);
    assert (account_one.m_balance == 80);
    assert (account_two.m_balance == 50);
}
}


namespace atomic_reference {
static void demo()
{
    // Non-atomic struct member.
    struct S { int val{}; };
    S s;

    // Make member atomic.
    const auto atomic_val = std::atomic_ref<int>(s.val);
    // Multiple threads can now operate atomically on the non-atomic struct member.
}
}


namespace barrier_jthread_stop_token {
// This function starts potentially failing processes.
// On any failure, it restarts the processes.
static void resilient_processes (const std::stop_token& stop_token)
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

    while (not stop_token.stop_requested())
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

static void demo()
{
    return; // Todo this crashes at times with segfault.
    std::jthread thread(resilient_processes);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // The thread goes out of scope here, so it generates the stop token for the process.
}
}


namespace thread_pool {

// Number of threads in the pool.
const std::size_t pool_size = std::thread::hardware_concurrency();

// Storage for the worker threads.
static std::vector<std::jthread> threads;

// Queue of tasks.
static std::queue<std::function<void()>> tasks;

// Mutex to synchronize access to shared data.
static std::mutex mutex;

// Condition variable to signal changes in the state of the tasks queue.
static std::condition_variable cv;

// Flag to indicate whether the thread pool should stop.
static std::atomic stop {false};

// The results of the tasks.
static std::mutex result_mutex;
static std::list<std::thread::id> result_thread_ids;
static std::atomic result_sum{0};
static std::atomic standard_sum{0};

static void start_thread_pool()
{
    // Creating worker threads.
    for (std::size_t i = 0; i < pool_size; ++i) {
        threads.emplace_back([] {
            while (true) {
                std::function<void()> task;
                // Put the code below in its scops.
                // On scope end, the queue is unlocked, enabling others to enqueue tasks.
                {
                    // Locking the queue so that data can be shared safely.
                    std::unique_lock<std::mutex> lock(mutex);

                    // Waiting until there is a task to execute or the pool is stopped.
                    // While in .wait it unlocks the mutex on the queue.
                    cv.wait(lock, [] {
                        return not tasks.empty() or stop;
                    });

                    // Exit the thread in case the pool is stopped and there are no tasks.
                    if (stop and tasks.empty()) {
                        return;
                    }

                    // Get the next task from the queue.
                    task = std::move(tasks.front());
                    tasks.pop();
                }

                // Run the task in this thread.
                task();
            }
        });
    }
}

static void stop_thread_pool()
{
    // Indicate stop.
    stop = true;

    // Notify all threads
    cv.notify_all();

    // Join all worker threads to ensure they have completed their tasks.
    threads.clear();
}


static void enqueue_task(std::function<void()> task)
{
    {
        std::unique_lock lock(mutex);
        tasks.emplace(std::move(task));
    }
    cv.notify_one();
}

static void demo()
{
    start_thread_pool();

    // Enqueue tasks for execution.
    for (int i = 0; i < pool_size; ++i) {

        // Store function without parameters.
        auto fn1 = [] {
            {
                std::unique_lock lock(result_mutex);
                result_thread_ids.push_back(std::this_thread::get_id());
            }
            result_sum += 1;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        };
        enqueue_task(fn1);
        standard_sum += 1;

        // Store lambda with a captured value, emulating one parameter.
        auto fn2 = [i]
        {
            {
                std::unique_lock lock(result_mutex);
                result_thread_ids.push_back(std::this_thread::get_id());
            }
            result_sum += i;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        };
        enqueue_task(fn2);
        standard_sum += i;

        // Store function with multiple parameters bound with std::bind.
        auto fn3 = [] (const int p1, const int p2, const int p3)
        {
            {
                std::unique_lock lock(result_mutex);
                result_thread_ids.push_back(std::this_thread::get_id());
            }
            result_sum += (p1 + p2 + p3);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        };
        int p1 = i;
        int p2 = p1 + 2;
        int p3 = p2 + 2;
        const auto fn3_bound = std::bind(fn3, p1, p2, p3);
        enqueue_task(fn3_bound);
        standard_sum += (p1 + p2 + p3);
    }

    stop_thread_pool();

    // Check whether all tasks ran.
    assert(result_thread_ids.size() == pool_size * 3);

    // Check all tasks executed the appropriate function.
    assert(result_sum == standard_sum);

    // Check whether all available threads were used.
    result_thread_ids.sort();
    result_thread_ids.unique();
    assert(result_thread_ids.size() == pool_size);
}
}


namespace lock_free {
// Type-trait to determine if the atomic object is implemented lock-free,
// that is, implemented directly using CPU instructions,
// without blocking or software locks like mutexes.
static void demo()
{
    struct Array { int a[100]; };
    struct Integer { int b; };

    assert(not std::atomic<Array>{}.is_lock_free());
    assert(    std::atomic<Integer>{}.is_lock_free());

    constexpr std::atomic<Array> aa;
    constexpr std::atomic<Integer> ai;
    assert(not std::atomic_is_lock_free(&aa));
    assert(    std::atomic_is_lock_free(&ai));
}
}



namespace memory_order {
// https://cppreference.com/cpp/atomic/memory_order

// If one thread writes to the atomic while another thread reads from it,
// the behaviour is defined by the memory order.
enum class memory_order
{
    relaxed,
    consume, // deprecated
    acquire,
    release,
    acq_rel,
    seq_cst,
};


static void demo()
{
    // Memory order relaxed.
    // There are no synchronization or ordering constraints imposed on other reads or writes,
    // of the same atomic variable, only this operation's atomicity is guaranteed.
    // Typical use: Incrementing counters (but not also decrementing them).
    {
        std::atomic atomic {0};
        {
            const auto func = [&atomic]
            {
                for (int i {0}; i < 1000; ++i)
                    atomic.fetch_add(1, std::memory_order::relaxed);
            };
            std::vector<std::jthread> threads{};
            for (int i {0}; i < 10; ++i)
                threads.emplace_back(func);
        }
        // Multiple threads correctly incremented the atomic.
        assert(atomic == 10 * 1000);
    }

    // Memory order consume (deprecated).
    // A load operation performs a consume operation on the affected memory location:
    // no reads or writes in the current thread dependent on the value currently loaded can be reordered before this load.
    // Writes to data-dependent variables in other threads that release the same atomic variable
    // are visible in the current thread.

    // Memory order acquire.
    // A load operation performs the acquire operation on the affected memory location:
    // no reads or writes in the current thread can be reordered before this load.
    // All writes in other threads that release the same atomic variable are visible in the current thread.

    // Memory order release.
    // A store operation performs the release operation:
    // no reads or writes in the current thread can be reordered after this store.
    // All writes in the current thread are visible in other threads that acquire the same atomic variable,
    // and writes that carry a dependency into the atomic variable become visible in other threads that consume the same atomic.

    // Release-Acquire ordering:
    // The producer and consumer operate on the same atomic running in two threads.
    // The producer tags the write as "release" and the consumer tags the read as "acquire".
    // This should guarantee that the consumer reads the value written by the producer.
    {
        std::atomic<std::string*> atomic{};

        std::string hello ("hello");

        const auto producer = [&]
        {
            atomic.store(&hello, std::memory_order::release);
        };

        const auto consumer = [&]
        {
            const std::string* ptr = atomic.load(std::memory_order::acquire);
            // In theory it should acquire the "hello" string.
            // But in practise it often fails to do so.
            assert(ptr == nullptr or ptr == &hello);
        };

        std::jthread {producer};
        std::jthread {consumer};
    }


    // Release-Consume ordering.
    // The producer and consumer operate on the same atomic running in two threads.
    // The producer tags the write as "release" and the consumer tags the read as "consume".
    // This should guarantee that the consumer reads the value written by the producer.
    {
        std::atomic<std::string*> atomic{};

        std::string hello ("hello");

        const auto producer = [&]
        {
            atomic.store(&hello, std::memory_order::release);
        };

        const auto consumer = [&]
        {
            const std::string* ptr = atomic.load(std::memory_order::consume);
            // It should consume the "hello" string.
            assert(ptr == &hello);
        };

        std::jthread {producer};
        std::jthread {consumer};
    }


    // Memory order acquire-release.
    // A read-modify-write operation is both an acquire operation and a release operation.
    // No memory reads or writes in the current thread can be reordered before the load, nor after the store.
    // All writes in other threads that release the same atomic variable are visible
    // before the modification and the modification is visible in other threads that acquire the same atomic variable.

    // Memory order sequential-consistent.
    // A load operation performs an acquire operation, a store performs a release operation,
    // and read-modify-write performs both an acquire operation and a release operation,
    // plus a single total order exists in which all threads observe all modifications in the same order.
    {
        std::atomic<std::string*> atomic{};

        std::string hello ("hello");

        const auto producer = [&]
        {
            atomic.store(&hello, std::memory_order::seq_cst);
        };

        const auto consumer = [&]
        {
            const std::string* ptr = atomic.load(std::memory_order::seq_cst);
            // It should consume the "hello" string.
            assert(ptr == &hello);
        };

        std::jthread {producer};
        std::jthread {consumer};
    }
}
}


void demo()
{
    atomic_wait::demo();
    atomic::demo();
    timed_mutex::demo();
    async_and_future::demo();
    packaged_task::demo();
    semaphores::demo();
    jthread::demo();
    condition_variables::demo();
    future_and_promise_and_exception::demo();
    execution_policies::demo();
    lock_multiple_simultaneously::demo();
    atomic_reference::demo();
    barrier_jthread_stop_token::demo();
    thread_pool::demo();
    lock_free::demo();
    memory_order::demo();
}
}
