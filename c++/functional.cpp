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

#include "functional.h"

#include <future>
#include <iostream>

namespace functional {


void move_only_function() {
    // https://en.cppreference.com/w/cpp/utility/functional/move_only_function.html
    // C++23 introduces the std::move_only_function,
    // a new utility for handling callable objects that don't need to be copyable.
    // It's a streamlined alternative to std::function,
    // designed for cases where you're working with move-only types
    // like std::unique_ptr or other non-copyable resources.

    std::packaged_task<double()> packaged_task([](){ return 3.14159; });

    std::future<double> future = packaged_task.get_future();

    auto lambda = [task = std::move(packaged_task)]() mutable { task(); };

    //  std::function<void()> function = std::move(lambda); // Error

    // std::move_only_function<void()> function = std::move(lambda); // OK does not yet compile on macOS 15.4.1

    //function();
    lambda();

    std::cout << future.get();

}


void demo() {
    //move_only_function();




}
}



