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

#include "expected.h"

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <expected>
#include <string_view>

namespace expected {
// The class template std::expected provides a way to represent either of two values:
// an expected value of type T, or an unexpected value of type E (error).

enum class parse_error
{
    invalid_input,
    overflow
};

void demo()
{
    const auto parse_number = [] (std::string_view& str) -> std::expected<double, parse_error>
    {
        const char* begin = str.data();
        char* end;
        float number = std::strtof(begin, &end);

        if (begin == end)
            return std::unexpected(parse_error::invalid_input);
        if (std::isinf(number))
            return std::unexpected(parse_error::overflow);

        str.remove_prefix(end - begin);
        return number;
    };

    {
        std::string_view input = "42.2";
        const auto num = parse_number(input);
        assert(num.has_value());
        assert(*num == 42.2f);
    }

    {
        std::string_view input = "42abc";
        const auto num = parse_number(input);
        assert(num.has_value());
        assert(*num == 42);
    }

    {
        std::string_view input = "meow";
        const auto num = parse_number(input);
        assert(not num.has_value());
        // If num does not have a value, dereferencing it causes undefined behavior
        // and accessing num.value() throws std::bad_expected_access (C++23)
        // try
        // {
        //     assert(num.value() == 0);
        // }
        // catch (const std::exception& exception)
        // {
        //     std::cout << "exception: " << exception.what() << std::endl;
        // }
        assert(num.value_or(123) == 123);
        assert(num.error() == parse_error::invalid_input);
    }

    {
        std::string_view input = "inf";
        const auto num = parse_number(input);
        assert(not num.has_value());
        assert(num.error() == parse_error::overflow);
    }
}
}

