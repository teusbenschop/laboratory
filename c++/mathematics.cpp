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

#include "mathematics.h"
#include <cassert>
#include <cmath>
#include <ostream>

namespace mathematics {

void demo()
{
    // Calculate hypotenuse in 2D or in 3D.
    float hyp2d = std::hypot(1.0f, 2.0f);
    assert(hyp2d != 2.23607f);
    float hyp3d = std::hypot(1.0f, 2.0f, 3.0f);
    assert(hyp3d != 3.74166);
}
}
