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


#include "forwarding.h"

#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

namespace forwarding {

// https://en.cppreference.com/w/cpp/utility/forward_like.html
// Returns a reference to x which has similar properties to T&&.

struct TypeTeller
{
    void operator()(this auto&& self)
    {
        using self_type = decltype(self);
        using unref_self_type = std::remove_reference_t<self_type>;
        if constexpr (std::is_lvalue_reference_v<self_type>)
        {
            if constexpr (std::is_const_v<unref_self_type>)
                std::cout << "const lvalue" << std::endl;
            else
                std::cout << "mutable lvalue" << std::endl;
        }
        if constexpr (std::is_rvalue_reference_v<self_type>) {
            if constexpr (std::is_const_v<unref_self_type>)
                std::cout << "const rvalue" << std::endl;
            else
                std::cout << "mutable rvalue" << std::endl;
        }
    }
};


struct FarStates
{
    std::unique_ptr<TypeTeller> pointer;
    std::optional<TypeTeller> optional;
    std::vector<TypeTeller> container;

    auto&& from_opt(this auto&& self)
    {
        return std::forward_like<decltype(self)>(self.optional.value());
        // It is OK to use std::forward<decltype(self)>(self).opt.value(),
        // because std::optional provides suitable accessors.
    }

    auto&& operator[](this auto&& self, std::size_t i)
    {
        return std::forward_like<decltype(self)>(self.container.at(i));
        // It is not so good to use std::forward<decltype(self)>(self)[i], because
        // containers do not provide rvalue subscript access, although they could.
    }

    auto&& from_ptr(this auto&& self)
    {
        if (!self.pointer)
            throw std::bad_optional_access{};
        return std::forward_like<decltype(self)>(*self.pointer);
        // It is not good to use *std::forward<decltype(self)>(self).ptr, because
        // std::unique_ptr<TypeTeller> always dereferences to a non-const lvalue.
    }
};

void demo()
{
    // return;

    FarStates my_state
    {
        .pointer{std::make_unique<TypeTeller>()},
        .optional{std::in_place, TypeTeller{}},
        .container{std::vector<TypeTeller>(1)},
      };

    my_state.from_ptr()();
    my_state.from_opt()();
    my_state[0]();

    std::cout << std::endl;

    std::as_const(my_state).from_ptr()();
    std::as_const(my_state).from_opt()();
    std::as_const(my_state)[0]();

    std::cout << std::endl;

    std::move(my_state).from_ptr()();
    std::move(my_state).from_opt()();
    std::move(my_state)[0]();

    std::cout << std::endl;

    std::move(std::as_const(my_state)).from_ptr()();
    std::move(std::as_const(my_state)).from_opt()();
    std::move(std::as_const(my_state))[0]();
}
}
