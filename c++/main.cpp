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


#include "bad_coding.h"
#include "bits.h"
#include "bounds_limits.h"
#include "classes.h"
#include "clocking.h"
#include "concurrency.h"
#include "constraints.h"
#include "containers.h"
#include "coroutines.h"
#include "counting.h"
#include "exceptions.h"
#include "expected.h"
#include "filesystem.h"
#include "functional.h"
#include "language.h"
#include "latches_barriers.h"
#include "linux.h"
#include "modules.h"
#include "searching.h"
#include "shared_mutex.h"
#include "templates.h"
#include "text.h"
#include "transformations.h"
#include "unions.h"
#include "variables.h"

int main()
{
    scoped_timer::demo();
    searching::demo();
    lower_bound_and_upper_bound::demo();
    counting::demo();
    min_max_clamp_ranges_minmax::demo();
    latches::demo();
    barriers::demo();
    unconstrained_errors::demo();
    constraint_derived_from::demo();
    demonstrate_constraints::demo();
    shared_mutex::demo();
    clocking::demo();
    filesystem::demo();
    coroutines::demo();
    modules::demo();
    concurrency::demo();
    containers::demo();
    language::demo();
    templates::demo();
    variables::demo();
    functional::demo();
    bad_coding::demo();
    expected::demo();
    text::demo();
    classes::demo();
    exceptions::demo();
    transformations::demo();
    bits::demo();
    linux::demo();
    unions::demo();
    return EXIT_SUCCESS;
}

