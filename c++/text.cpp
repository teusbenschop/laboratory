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


#include <cassert>
#include <format>
#include <iomanip>
#include <iostream>
#include <numbers>
#include <ranges>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "text.h"
#include "clocking.h"


namespace text {

namespace character_literals {
// Consistent character literal encoding.
// ReSharper disable once CppIdenticalOperandsInBinaryExpression
static_assert('A' == '\x41');
static_assert('A' == 0x41);
}


namespace escape_sequences {
static void demo() {
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
static void demo()
{
    {
        const std::string result = std::format("c={} s={} 1={}", "c", std::string("s"), 1);
        assert(result == "c=c s=s 1=1");
    }
    {
        // Numbered replacement fields:
        // - Can be out of order.
        // - Can format arguments(s) more than once.
        const std::string result = std::format("{2} {1} {0} {1} {2}", "a", "b", "c");
        assert (result == "c b a b c");
    }
    {
        // Format specifiers for fill and align.
        constexpr char c = 120;
        assert(std::format("{:6}", 42)    == "    42");
        assert(std::format("{:6}", 'x')   == "x     ");
        assert(std::format("{:_<6}", 'x') == "x_____");
        assert(std::format("{:_>6}", 'x') == "_____x");
        assert(std::format("{:_^6}", 'x') == "__x___");
        assert(std::format("{:6d}", c)    == "   120");
        assert(std::format("{:6}", true)  == "true  ");
        // Format specifiers for sign and space.
        // space: Use a leading space for non-negative numbers.
        //        Use a minus sign for negative numbers.
        double inf = std::numeric_limits<double>::infinity();
        double nan = std::numeric_limits<double>::quiet_NaN();
        assert(std::format("{0:},{0:+},{0:-},{0: }", 1)   == "1,+1,1, 1");
        assert(std::format("{0:},{0:+},{0:-},{0: }", -1)  == "-1,-1,-1,-1");
        assert(std::format("{0:},{0:+},{0:-},{0: }", inf) == "inf,+inf,inf, inf");
        assert(std::format("{0:},{0:+},{0:-},{0: }", nan) == "nan,+nan,nan, nan");
        // Format specifiers for width and precision.
        float pi = 3.14f;
        assert(std::format("{:10f}", pi)           == "  3.140000"); // width = 10
        assert(std::format("{:{}f}", pi, 10)       == "  3.140000"); // width = 10
        assert(std::format("{:.5f}", pi)           == "3.14000");    // precision = 5
        assert(std::format("{:.{}f}", pi, 5)       == "3.14000");    // precision = 5
        assert(std::format("{:10.5f}", pi)         == "   3.14000"); // width = 10, precision = 5
        assert(std::format("{:{}.{}f}", pi, 10, 5) == "   3.14000"); // width = 10, precision = 5
    }
    {
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
}


namespace vformat_demo {
// Takes dynamic format strings.
template<typename... Args>
static std::string func(const std::format_string<Args...> fmt, Args&&... args)
{
    return std::vformat(fmt.get(), std::make_format_args(args...));
}

static void demo()
{
    const std::string s1 = func("{}{} {}{}", "Hello", ',', "C++", -1 + 2 * 3 * 4);
    assert (s1 == "Hello, C++23");
    const std::string dynamic_format_string = "User [{1}] triggered event ID: {0}";

    const auto func = [](const std::string& dynamic_fmt, int id, std::string_view user) {
        // std::format(dynamic_fmt, id, user) would fail to compile here
        return std::vformat(dynamic_fmt, std::make_format_args(id, user));
    };
    std::string config_fmt = "User {1} logged in with id {0}";
    assert(func(config_fmt, 123, "Foo") == "User Foo logged in with id 123");
}
}


namespace stream_manipulation {
static void demo()
{
    {
        std::ostringstream oss;
        oss << std::boolalpha << false << " " << std::noboolalpha << true;
        assert(oss.str() == "false 1");
    }
    {
        std::istringstream iss {1};
        int i;
        iss >> i;
        assert(iss.eof()); // Read till end-of-file.
    }
    {
        std::istringstream iss {"a"};
        int i;
        iss >> i;
        assert(iss.fail()); // Failed to put "a" to int.
    }
    {
        std::istringstream iss;
        iss.clear();
        iss.setstate(
            std::ios_base::goodbit // no error
            | std::ios_base::badbit // irrecoverable stream error
            | std::ios_base::failbit // formatting or extraction error
            | std::ios_base::eofbit); // end-of-file reached.
    }
    // The std::istream_iterator and std::ostream_iterator are adapters
    // to treat input and output streams as ranges.
    {
        std::istringstream input("10 20 30 40 50");

        std::istream_iterator<int> begin_iter(input);
        std::istream_iterator<int> end_iter; // Default constructor represents end-of-stream.

        // Fill a container via both stream iterators.
        std::vector<int> numbers(begin_iter, end_iter);

        // Output the read via an ostream_iterator.
        std::ostringstream output;
        // The second argument " " acts as a delimiter after every item written.
        std::ostream_iterator<int> output_stream(output, " ");

        std::ranges::copy(numbers, output_stream);
        assert(output.str() == "10 20 30 40 50 ");
    }
}
}


namespace stream_str_view {

struct str {}; struct view{};
constexpr size_t count {1000};

template <typename T>
static void test(const std::string& input)
{
    scoped_timer::scoped_timer<std::chrono::microseconds> timer;
    for (std::decay_t<decltype(count)> i {0}; i < count; ++i)
    {
        std::stringstream ss{input};
        if constexpr (std::is_same_v<T, str>)
            auto s = ss.str(); // Create copy -> slower
        if constexpr (std::is_same_v<T, view>)
            auto s = ss.view(); // Return string_view, no copy -> faster.
    }
}

static void demo()
{
    const std::string input ('a', count);
    // test<str>(input);
    // test<view>(input);
}
}

namespace osyncstream {
// https://en.cppreference.com/w/cpp/io/basic_osyncstream
// The class template std::basic_osyncstream is a convenience wrapper for std::basic_syncbuf.
// It provides a mechanism to synchronize threads writing to the same stream.
static void demo()
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
static void demo()
{
    {
        // When filling up put the value at the left.
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
static void demo() {
    // Get the floats from the input string.
    {
        auto iss = std::istringstream{"1.4142 1.618 2.71828 3.14159 6.283"};
        auto&& floats = std::ranges::istream_view<float>(iss);
        auto result = floats | std::ranges::to<std::vector<float>>();
        std::vector<float> standard = {1.4142, 1.618, 2.71828, 3.14159, 6.283};
        assert(result == standard);
    }
    // Get the separate words.
    {
        auto iss = std::istringstream{"how \f was \n yesterday's \t weather?"};
        auto&& strings = std::ranges::istream_view<std::string>{iss};
        const auto result = strings | std::ranges::to<std::vector<std::string>>();
        std::vector<std::string> standard = {"how", "was", "yesterday's", "weather?"};
        assert(result == standard);
    }
}
}


namespace templates_printf {

// The base function if no arguments are given, only the format string.
static void template_print_format(std::ostringstream& oss, const char* format)
{
    oss << format;
}

// The recursive variadic function.
template <typename T, typename... Targs>
static void template_print_format(std::ostringstream& oss, const char* format, T value, Targs... args)
{
    for (; *format; ++format)
    {
        if (*format == '%')
        {
            oss << value;
            template_print_format(oss, format + 1, args...);
            return;
        }
        oss << *format;
    }
}

static void demo()
{
    std::ostringstream oss;
    template_print_format(oss, "% world % %", "Hello", "!", 123);
    assert(oss.str() == "Hello world ! 123");
}
}


namespace string_literals {
// Ordinary literal encoding
const char* const cc1 = "abc";
// UTF-8 string.
const char8_t* const cc2 = u8"abc";
// Raw string literal UTF-8.
const char8_t* const cc3 = u8R"(abc)";
// Adjacent string literal are concatenated by the compiler.

// The std::string string literal.
using namespace std::literals;
static auto hello = "hello"s;
static_assert(std::is_same_v<decltype(hello), std::string>);

static void demo()
{
    // String literals may have embedded null characters but strlen fails on that.
    const char* const cc4 = "abc\0abc";
    static_assert(sizeof(cc4) == 8); // including \0 at the end.
    assert(strlen(cc4) == 3);
}
}


namespace user_defined_literals {

// Create user-defined types by defining a user-defined suffix.

class Distance
{
public:
    constexpr static long double km_per_mile = 1.609344L;
    long double get_kilometers() const { return kilometers; };

    Distance operator+(const Distance other)
    {
        return Distance(get_kilometers() + other.get_kilometers());
    }

private:
    explicit Distance (const long double val) : kilometers(val) {}
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

static void demo_distance()
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

static void demo()
{
    demo_distance();
}
}


namespace string_operator_square_brackets_versus_dot_at {

// The string operator [] does not do bounds checking -> faster / unsafe.
// The string operator at() does bounds checking -> slower / safe.
constexpr unsigned size {100000};
namespace { struct brackets{}; struct at{}; }

template <typename T>
static void speed_test()
{
    //scoped_timer::scoped_timer<std::chrono::microseconds> timer;
    const std::string input ("1", size);
    for (unsigned i = 0; i < size; ++i)
    {
        if constexpr (std::is_same_v<T, brackets>)
            const char c = input[i];
        if constexpr (std::is_same_v<T, at>)
            const char c = input.at(i);
    }
}

static void demo()
{
    speed_test<brackets>();
    speed_test<at>();
    std::string s;
}
}


namespace logging {
// The std::clog write to std::cerr and is buffered unlike std::cerr.
// Example: std::clog << 1;
static void demo()
{
}
}


void demo() {
    escape_sequences::demo();
    formatting_library::demo();
    vformat_demo::demo();
    stream_manipulation::demo();
    stream_str_view::demo();
    osyncstream::demo();
    output_manipulation::demo();
    istream_view::demo();
    templates_printf::demo();
    string_literals::demo();
    user_defined_literals::demo();
    string_operator_square_brackets_versus_dot_at::demo();
    logging::demo();
}


}

