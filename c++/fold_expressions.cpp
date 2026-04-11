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


#include "fold_expressions.h"

namespace fold_expressions {

template <typename ... Args>
constexpr int subtract_right_fold(Args&& ... args)
{
    // (arg1 - (... - (argN-1 - argN)))
    return (args - ...);
}
// (1 - (2 - 3))
static_assert(subtract_right_fold(1,2,3) == 2);

template <typename ... Args>
constexpr int subtract_left_fold(Args&& ... args)
{
    // (((arg1 op arg2) op ...) op argN)
    return (... - args);
}
// ((1 - 2) - 3)
static_assert(subtract_left_fold(1,2,3) == -4);

template <typename I, typename ... Args>
constexpr int init_subtract_right_fold(I init, Args&& ... args)
{
    // (arg1 op (... op (argN−1 op (argN op I))))
    return (args - ... - init);
}
// (1 - (2 - (3 - 10)))
static_assert(init_subtract_right_fold(10, 1,2,3) == -8);

template <typename I, typename ... Args>
constexpr int init_subtract_left_fold(I init, Args&& ... args)
{
    // ((((init - arg1) - arg2) - ...) op argN)
    return (init - ... - args);
}
// ((10 - 1) - 2) - 3
static_assert(init_subtract_left_fold(10, 1,2,3) == 4);

void demo()
{
}

}
