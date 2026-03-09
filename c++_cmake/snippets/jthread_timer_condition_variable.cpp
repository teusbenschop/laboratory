#include <condition_variable>
#include <iostream>
#include <thread>
#include <mutex>
#include "functions.h"

// This demonstrates a repeating timer that can be used to run repeating tasks.
// It uses a timed mutex and a condition variable.
// The timer runs in a joining thread, a jthread.
// This jthread caters for starting and stopping the timer.

namespace snippets {

void demo_timer_with_jthread_timed_mutex_condition_variable()
{
  std::timed_mutex mx;
  std::condition_variable_any cv;

  // This lambda function takes a stop token as parameters.
  const auto timer = [&](const std::stop_token& stoken) {
    std::cout << "Step 3: Set the timer interval to 100 milliseconds" << std::endl;
    constexpr auto interval = std::chrono::milliseconds(100);
    while (!stoken.stop_requested()) {
      std::cout << "Step 4: The stop token has no stop request: Keep going" << std::endl;
      std::unique_lock ulk(mx);
      std::cout << "Step 5: Enter the condition variable which will wait 100 ms or less in case of a thread stop request" << std::endl;
      if (cv.wait_for(ulk, stoken, interval, [&stoken] { return stoken.stop_requested();})) {
        std::cout << "Step 8: The condition variable got a stop request and so interrupts its waiting state immediately" << std::endl;
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
  std::cout << "Step 7: The jthread will go out of scope, this sends a stop request to the thread function" << std::endl;
}

}

