#include <cstdlib>
#include <cstddef>
#include <thread>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <random>
#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <sstream>
#include "main.h"

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    std::cout << "start" << std::endl;
    while (true) {
//        reset_boost_timer ();
//        clear_integer ();
        reset_condition_variable ();
//        heartbeat_cpp ();

    }
    std::cout << "finish" << std::endl;
    return EXIT_SUCCESS;
}


void reset_boost_timer ()
{
    boost::asio::io_service io_service {};
    
    constexpr int heartbeat_seconds {2};
    std::atomic <int> message_timeout {0};
    
    auto expired_heartbeat_timer = std::make_shared<boost::asio::deadline_timer>(io_service);
    std::function<void(const boost::system::error_code &ec)> on_expired_heartbeat =
    [&expired_heartbeat_timer, &on_expired_heartbeat, &heartbeat_seconds, &message_timeout]
    (const boost::system::error_code &ec) -> void
    {
        if (ec) return;
        expired_heartbeat_timer->expires_from_now (boost::posix_time::seconds(heartbeat_seconds));
        expired_heartbeat_timer->async_wait (on_expired_heartbeat);
        std::cout << "message timeout reached" << std::endl;
    };
    expired_heartbeat_timer->expires_from_now(boost::posix_time::seconds(heartbeat_seconds));
    expired_heartbeat_timer->async_wait(on_expired_heartbeat);
    
    bool run_thread {true};
    std::thread message_simulation_thread ([&expired_heartbeat_timer, &on_expired_heartbeat, &heartbeat_seconds, &message_timeout, &run_thread] {
        while (run_thread) {
            expired_heartbeat_timer->expires_from_now (boost::posix_time::seconds(heartbeat_seconds));
            expired_heartbeat_timer->async_wait (on_expired_heartbeat);
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        }
    });
    
    io_service.run();
    
    run_thread = false;
    message_simulation_thread.join ();
}


void clear_integer ()
{
    boost::asio::io_service io_service {};
    
    constexpr int heartbeat_seconds {2};
    std::atomic <int> message_timeout {0};

    auto expired_heartbeat_timer = std::make_shared<boost::asio::deadline_timer>(io_service);
    std::function<void(const boost::system::error_code &ec)> on_expired_heartbeat =
    [&expired_heartbeat_timer, &on_expired_heartbeat, &heartbeat_seconds, &message_timeout]
    (const boost::system::error_code &ec) -> void
    {
        if (ec) return;
        expired_heartbeat_timer->expires_from_now (boost::posix_time::seconds(heartbeat_seconds));
        expired_heartbeat_timer->async_wait (on_expired_heartbeat);
        message_timeout++;
        if (message_timeout > 5) {
            std::cout << "message timeout reached" << std::endl;
            message_timeout = 0;
        }
    };
    expired_heartbeat_timer->expires_from_now(boost::posix_time::seconds(heartbeat_seconds));
    expired_heartbeat_timer->async_wait(on_expired_heartbeat);
    
    bool run_thread {true};
    std::thread message_simulation_thread ([&expired_heartbeat_timer, &on_expired_heartbeat, &heartbeat_seconds, &message_timeout, &run_thread] {
        while (run_thread) {
            message_timeout = 0;
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        }
    });
    
    io_service.run();
    
    run_thread = false;
    message_simulation_thread.join ();
}


void reset_condition_variable ()
{
    
    constexpr int heartbeat_seconds {2};

    std::mutex cv_m;
    std::condition_variable cv;
    bool is_connected {true};
    bool keep_running {true};

    std::thread worker ([&cv_m, &cv, &heartbeat_seconds](const bool& is_connected, const bool& keep_running) {
        while (keep_running) {
            std::unique_lock ulk (cv_m);
            if (cv.wait_for(ulk, std::chrono::seconds(heartbeat_seconds)) == std::cv_status::timeout && keep_running && is_connected) {
                std::cout << "Timeout expired. Sending heartbeat message..." << std::endl;
            }
            
            if (keep_running) {
  //              std::cout << "Resetting timer." << std::endl;
            }
            
        }
    }, std::cref(is_connected), std::cref(keep_running));

    
    bool run_thread {true};
    std::thread message_simulation_thread ([&run_thread, &cv_m, &is_connected, &cv] {
        while (run_thread) {
            {
                std::lock_guard lg(cv_m);
                is_connected = true;
            }
            cv.notify_all();
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        }
    });
    
    std::this_thread::sleep_for(std::chrono::seconds(100));
    run_thread = false;
    message_simulation_thread.join ();
    worker.join ();
}


void heartbeat_cpp ()
{
    std::mutex cv_m;
    std::condition_variable cv;
    bool is_connected {true};
    bool keep_running {true};
    
    std::thread worker([&cv_m, &cv](const bool& is_connected, const bool& keep_running) {
        while (keep_running) {
            std::unique_lock ulk(cv_m);
            if ( cv.wait_for(ulk, std::chrono::seconds(3)) == std::cv_status::timeout && keep_running && is_connected)
                std::cout << "Timeout expired. Sending heartbeat message...";
            
            if (keep_running)
                std::cout << "Resetting timer.\n";
            
            
        }
    }, std::cref(is_connected), std::cref(keep_running));
    
    std::cout << "Enter '1' to simulate sending a message. '2' for closing the connection, or any other key to quit:\n";
    
    while (keep_running) {
        int c;
        std::cin >> c;
        
        if (c == 1) {
            std::cout << "Sending message ... ";
            {
                std::lock_guard lg(cv_m);
                is_connected = true;
            }
            cv.notify_all();
        } else if (c == 2) {
            std::cout << "Disconnecting ...\n ";
            {
                std::lock_guard lg(cv_m);
                is_connected = false;
            }
            cv.notify_all();
        } else {
            {
                std::lock_guard lg(cv_m);
                is_connected = false;
                keep_running = false;
            }
            cv.notify_all();
            std::cout << "Quitting...\n";
        }
    }
    
    worker.join();
    std::cout << "Program complete.\n";

}
