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


#include "text.h"

#include <cassert>
#include <format>
#include <iomanip>
#include <iostream>
#include <numbers>
#include <ranges>
#include <string>
#include <sstream>
#include <thread>
#include <vector>


namespace text {

namespace character_literals {
// Consistent character literal encoding.
// ReSharper disable once CppIdenticalOperandsInBinaryExpression
static_assert('A' == '\x41');
static_assert('A' == 0x41);
}


namespace escape_sequences {

void demo() {
    // Named universal character escapes.
    // https://www.unicode.org/Public/14.0.0/ucd/NamesList.txt
    assert("\N{CAT FACE}"      == std::string("🐱"));
    assert("\N{COW FACE}"      == std::string("🐮"));
    assert("\N{NATIONAL PARK}" == std::string("🏞"));

    // Delimited escape sequences.
    assert("\o{111}"  == std::string("I"));
    assert("\x{A0}"   != std::string(""));
    assert("\u{CAFE}" == std::string("쫾"));

}

}


namespace formatting_library {
// https://en.cppreference.com/w/cpp/utility/format
void demo()
{
    std::string result = std::format("A={} B={} C={}", "a", std::string("b"), 3);
    assert(result == "A=a B=b C=3");

    // Formats to an output iterator.
    std::string buffer;
    std::format_to
    (
        std::back_inserter(buffer), // the output iterator.
        "Hello, C++{}!", // the format string.
        20 // the argument(s).
    );
    assert(buffer == "Hello, C++20!");
}
}


namespace stream_manipulation {
void demo()
{
    {
        std::stringstream oss;
        oss << std::boolalpha << false << " " << std::noboolalpha << true;
        assert(oss.str() == "false 1");
    }
}
}


namespace osyncstream {
// https://en.cppreference.com/w/cpp/io/basic_osyncstream
// The class template std::basic_osyncstream is a convenience wrapper for std::basic_syncbuf.
// It provides a mechanism to synchronize threads writing to the same stream.
void demo()
{
    const auto stream_worker = []([[maybe_unused]] int id) {
        using namespace std::literals::chrono_literals;
        for (int i = 0; i < 2; i++) {
            std::this_thread::sleep_for(1ms);
            // std::osyncstream synced_out(std::cout);
            // synced_out << "worker " << id << std::endl;
        }
    };

    std::jthread threads [4];
    for (int i = 0; i < 4; ++i) {
        threads[i] = std::jthread(stream_worker, i);
    }

}
}




namespace output_manipulation {
void demo()
{
    {
        // When filling put the value at the left.
        std::stringstream ss;
        ss << std::left << std::setfill('_') << std::setw(10) << -1.23;
        assert(ss.str() == "-1.23_____");
    }
    {
        // When filling up put the value at the right (this is the default normally).
        std::stringstream ss;
        ss << std::right << std::setfill('_') << std::setw(10) << -1.23;
        assert(ss.str() == "_____-1.23");
    }
}
}


namespace istream_view {
void demo() {
    // Demo of istream_view.
    {
        const auto s{"1.4142 1.618 2.71828 3.14159 6.283"};
        auto iss = std::istringstream{s};
        auto floats = std::ranges::istream_view<float>(iss);
        auto result = floats | std::ranges::to<std::vector<float>>();
        std::vector<float> standard = {1.4142, 1.618, 2.71828, 3.14159, 6.283};
        assert(result == standard);
    }
}
}


namespace templates_printf {

// The base function if no arguments are given, only the format string.
void template_printf(const char* format)
{
    std::cout << format;
}

// The recursive variadic function.
template <typename T, typename... Targs>
void template_printf(const char* format, T value, Targs... args)
{
    for (; *format; ++format)
    {
        if (*format == '%')
        {
            std::cout << value;
            template_printf(format + 1, args...);
            return;
        }
        std::cout << *format;
    }
}

void demo()
{
    //template_printf("% world % %", "Hello", "!", 123);
}
}

namespace string_literals {
// Ordinary literal encoding
const char* cc1 = "abc";
// UTF-8 string.
const char8_t* cc2 = u8"abc";
// Raw string literal UTF-8.
const char8_t* cc3 = u8R"(abc)";

// Adjacent string literal are concatenated by the compiler.

void demo()
{
    // String literals may have embedded null characters but strlen fails on that.
    const char* cc4 = "abc\0abc";
    static_assert(sizeof(cc4) == 8); // including \0 at the end.
    assert(strlen(cc4) == 3);
}
}


namespace user_defined_literals {

// Create user-defined types by defining a user-defined suffix.

struct Distance
{
public:
    constexpr static long double km_per_mile = 1.609344L;
    long double get_kilometers() const { return kilometers; };

    Distance operator+(const Distance other)
    {
        return Distance(get_kilometers() + other.get_kilometers());
    }

private:
    explicit Distance (long double val) : kilometers(val) {}
    long double kilometers{0.0};
    friend Distance operator""_km(long double val);
    friend Distance operator""_mi(long double val);
};

Distance operator""_km(long double val)
{
    return Distance(val);
}

Distance operator""_mi(long double val)
{
    return Distance(val * Distance::km_per_mile);
}

void demo_distance()
{
    // Must have a decimal point to bind to the operator we defined.
    Distance d{ 402.0_km }; // construct using kilometers
    assert(d.get_kilometers() == 402.0);

    Distance d2{ 402.0_mi }; // construct using same amount of miles
    assert(d2.get_kilometers() > 646.955 and d2.get_kilometers() < 646.957);

    // Add distances constructed with different units.
    Distance d3 = 36.0_mi + 42.0_km;
    assert(d3.get_kilometers() > 99.9363 and d3.get_kilometers() < 99.9365);

    // Distance d4(90.0); // error constructor not accessible
}


constexpr long double operator""_degrees_to_radians(long double degrees)
{
    long double radians = degrees * std::numbers::pi_v<long double> / 180;
    return radians;
}

static_assert(90.0_degrees_to_radians >= 1.57 and 90.0_degrees_to_radians <= 1.58);

void demo()
{
    demo_distance();
}
}


void demo() {
    escape_sequences::demo();
    formatting_library::demo();
    stream_manipulation::demo();
    osyncstream::demo();
    output_manipulation::demo();
    istream_view::demo();
    templates_printf::demo();
    string_literals::demo();
    user_defined_literals::demo();
}


}

