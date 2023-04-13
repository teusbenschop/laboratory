#pragma once

#include <string_view>
#include <map>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>


class stuck
{
public:
    stuck (int id);
    virtual ~stuck ();
    std::thread m_worker_thread{};
    std::atomic_bool m_worker_quit {false};
    void loop ();
    int id {0};
    std::atomic_bool m_ready {false};
};


