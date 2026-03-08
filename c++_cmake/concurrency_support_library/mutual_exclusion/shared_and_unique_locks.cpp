#include <condition_variable>
#include <iostream>
#include <queue>
#include <shared_mutex>
#include <thread>
#include "functions.h"

namespace concurrency_support_library::mutual_exclusion {

// This example code features a queue.
// Some code writes to it, and some code reads from it.
// To synchronize the read and write operations, the following locks are used:
// 1. A shared lock to enable one or more processes to read from the queue simultaneously.
// 2. A unique lock so only one bit of code can write to the queue.

static std::queue<int> the_queue;
static std::shared_mutex the_mutex;
static std::condition_variable_any the_cv;


void print_queue(const std::stop_token stoken)
{
    while (!stoken.stop_requested()) {
        // Wait for something to be broadcast or for a stop request.
        // This uses a unique lock because it might modify the queue.
        std::unique_lock lock(the_mutex);
        the_cv.wait_for(lock, stoken, std::chrono::seconds(1), [&stoken] {
          return stoken.stop_requested() || !the_queue.empty();
        });

        // If a stop is requested, do that right now.
        if (stoken.stop_requested())
            break;

        // Print / remove the value at the front of the queue.
        std::cout << "Removing " << the_queue.front() << " from queue" << std::endl;
        the_queue.pop();
    }
}


bool queue_empty()
{
    // This uses a shared lock because it does not modify the queue.
    std::shared_lock lock(the_mutex);
    return the_queue.empty();
}


void shared_and_unique_locks()
{
    std::jthread printer(print_queue);

    for (int i {1}; i <= 5; i++) {
        // This uses a unique lock because it modifies the queue.
        std::cout << "Attempt to obtain a unique lock" << std::endl;
        std::unique_lock lock(the_mutex);
        std::cout << "The unique lock was obtained" << std::endl;
        std::cout << "Push " << i << " onto queue" << std::endl;
        the_queue.push(i);
    }

    // Wait till the queue is empty.
    while (!queue_empty())
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
}


}
