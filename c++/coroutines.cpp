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

#include "coroutines.h"
#include <coroutine>
#include <exception>
#include <iostream>
#include <ostream>

namespace coroutines {
// https://en.cppreference.com/w/cpp/language/coroutines
// Coroutines are designed to be performing as lightweight threads.
// Coroutines provide concurrency but not parallelism.
// Switching between coroutines need not involve any system/blocking calls
// so no need for synchronization primitives such as mutexes, semaphores.
// A coroutine does not have a stack, it stores its variables on the heap.
// A coroutine suspends execution by returning to the caller.
// The data needed to resume operations is on the heap.
// A function is a coroutine if its definition contains any of the following:
// * co_await: suspend execution until resumed.
// * co_yield: suspend execution returning a value.
// * co_return: complete execution returning a value.

namespace example1 {

struct returner
{
    struct promise_type
    {
        returner get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() { }
    };
};

struct awaiter
{
    std::coroutine_handle<>* m_handle;
    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> handle) { *m_handle = handle; }
    void await_resume() { }
};

// This function runs forever. It increases and prints the value.
// The variable i maintains its value even as control switches repeatedly
// between this function and the function that invoked it.
returner infinite_counter(std::coroutine_handle<>* handle) {
    awaiter a{handle};
    int i {0};
    while (true)
    {
        // Suspend the coroutine and returns control to the caller.
        co_await a;
        std::cout << "in coroutine " << ++i << std::endl;
    }
}

void demo()
{
    std::coroutine_handle<> handle;
    infinite_counter(&handle);
    for (int i = 0; i < 3; ++i)
    {
        std::cout << "in main function" << std::endl;
        handle();
    }
    handle.destroy();
}
}

namespace simple_generator {


template <typename T>
class Generator {
public:
    // ===========================================================
    //  THE PROMISE TYPE -- The "control panel" for the coroutine
    // ===========================================================
    struct promise_type {
        T current_value;  // Stores the most recently yielded value

        // What to return to the caller when the coroutine is created
        Generator get_return_object() {
            return Generator{
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }

        // Suspend immediately -- don't run the body until asked
        std::suspend_always initial_suspend() noexcept { return {}; }

        // Suspend at the end -- let the Generator destructor clean up
        std::suspend_always final_suspend() noexcept { return {}; }

        // When the coroutine does: co_yield value;
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }

        // Our generator doesn't return a final value, just yields
        void return_void() {}

        // If something throws inside the coroutine body
        void unhandled_exception() {
            std::terminate();  // Nuclear option. You can rethrow instead.
        }
    };

    // ======================================================
    //  THE GENERATOR CLASS -- What the caller interacts with
    // ======================================================

    explicit Generator(std::coroutine_handle<promise_type> handle)
        : m_handle(handle) {}

    ~Generator() {
        if (m_handle) {
            m_handle.destroy();
        }
    }

    // No copying! The handle is a unique resource.
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    // Move is fine though
    Generator(Generator&& other) noexcept
        : m_handle(std::exchange(other.m_handle, nullptr)) {}

    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (m_handle) m_handle.destroy();
            m_handle = std::exchange(other.m_handle, nullptr);
        }
        return *this;
    }

    // Advance to the next value. Returns false if done.
    bool next() {
        m_handle.resume();
        return not m_handle.done();
    }

    // Get the current yielded value
    T value() const {
        return m_handle.promise().current_value;
    }

private:
    std::coroutine_handle<promise_type> m_handle;
};

Generator<int> count_up_to(int max) {
    for (int i = 1; i <= max; ++i) {
        co_yield i;  // "Here's a value. Now pause me."
    }
    // Implicit co_return at the end.
}

void demo()
{
    auto gen = count_up_to(5);
    while (gen.next()) {
        std::cout << gen.value() << std::endl;  // Prints 1, 2, 3, 4, 5
    }
}
}

void demo()
{
    // example1::demo();
    // simple_generator::demo();
}
}

