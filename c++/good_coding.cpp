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

#include "good_coding.h"

#include <algorithm>
#include <array>

namespace good_coding {

// Avoid the dangling iterator.
// The iterator points to an element in the array.
// By the time the iterator is obtained, the array has gone out of scope.
// Hence, the iterator is dangling.
auto dangling_iter = std::ranges::max_element(std::array{0, 1, 0, 1});
static_assert(std::is_same_v<std::ranges::dangling, decltype(dangling_iter)>);
// std::cout << *dangling_iter;
// compilation error: no match for 'operator*' (operand type is 'std::ranges::dangling')


// Make interfaces easy to use properly and hard to use incorrectly.
// Consistency in interfaces like in STL.
// Behavioral compatibility with built-in types.
// Factory functions return smart pointer for automatic delete.


// Treat class design as type design.
// * Consider construction and destruction.
// * How does object initialization differ from object assignment.
// * Implications of passing object by value.
// * Restrictions on legal values.
// * Does it inherit or provide base class.
// * Allowed type conversions: implicit or explicit.
// * Which operators and methods are needed.
// * What is public / private / protected.
// * Is class template better.


// Declare data members private.


// Prefer non-member non-friend functions to member functions (increases encapsulation).


// Declare non-member functions when type conversions are needed for all parameters to that function.


// A swap function.
// * create swap member function.
// * non-member swap to call member swap.
// * on call: use 'using' declaration to let compiler choose the best swap.


void demo() {
}
}
