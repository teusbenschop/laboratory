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

struct return_object {
  struct promise_type {
    return_object get_return_object() { return {}; }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void unhandled_exception() {}
  };
};

struct awaiter {
  std::coroutine_handle<> *m_handle;
  constexpr bool await_ready() const noexcept { return false; }
  void await_suspend(std::coroutine_handle<> handle) { *m_handle = handle; }
  constexpr void await_resume() const noexcept {}
};

// This function runs forever. It increases and prints the value.
// The variable i maintains its value even as control switches repeatedly
// between this function and the function that invoked it.
return_object infinite_counter(std::coroutine_handle<>* handle) {
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
    return;
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

