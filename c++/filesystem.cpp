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
#include <string>

namespace filesystem {

void demo()
{
    {
        std::filesystem::path p("/var/log/app");
        const auto dirname = p.parent_path().string();
        assert(dirname == "/var/log");
    }

    assert(std::filesystem::path::preferred_separator == '/');

    {
        std::filesystem::path p("log");
        std::filesystem::remove(p);
    }

    {
        std::filesystem::path path("/tmp/hi.txt");
        assert(std::filesystem::exists (path) == false);
    }

    std::filesystem::path path(R"(/tmp)");
    for (const auto& directory_entry : std::filesystem::directory_iterator{path})
    {
        std::filesystem::path path = directory_entry.path();
        assert(!path.empty());
    }
}

}
