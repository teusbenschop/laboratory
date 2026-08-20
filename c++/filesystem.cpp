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

#include "filesystem.h"
#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

namespace filesystem {

static_assert(std::filesystem::path::preferred_separator == '/');

void demo()
{
    {
        std::filesystem::path p("/var/log/app");
        const auto dirname = p.parent_path().string();
        assert(dirname == "/var/log");
    }

    {
        std::filesystem::path p("log");
        std::filesystem::remove(p);
    }

    {
        std::filesystem::path path("/tmp/hi.txt");
        assert(std::filesystem::exists (path) == false);
    }

    std::filesystem::path path("/tmp");
    for (const auto& directory_entry : std::filesystem::directory_iterator{path})
    {
        std::filesystem::path file = directory_entry.path();
        assert(!file.empty());
    }

    try
    {
        std::filesystem::copy("a", "b"); // throws.
    }
    catch (...) { }
    std::error_code ec;
    std::filesystem::copy("a", "b", ec); // does not throw but fills error code.
    assert(ec.value() == 2);
    assert(ec.message() == "No such file or directory");
}

}
