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


#include "clocking.h"
#include <cassert>
#include <iomanip>
#include <thread>

namespace scoped_timer {

void demo()
{
    return;
    const auto timer = scoped_timer<std::chrono::microseconds>{};
    std::this_thread::sleep_for(std::chrono::microseconds(100));
}

}


namespace clocking {
// https://en.cppreference.com/w/cpp/chrono
// The chrono library defines five main types:
// clocks
// time points
// durations
// calendar dates
// time zone information
void demo()
{
    // A clock consists of a starting point (epoch) and a tick period in seconds.
    // E.g. a clock may have an epoch of 1st January 1970 UTC and tick every second.

    // The system_clock is the real-time wall clock.
    static_assert(std::chrono::system_clock::period::num == 1);
    static_assert(not std::chrono::system_clock::is_steady);

    // The steady_clock is monotonic and never gets adjusted.
    static_assert(std::chrono::steady_clock::period::num == 1);
    static_assert(std::chrono::steady_clock::is_steady);

    // The high_resolution_clock is a clock with the shortest possible tick period.
    // - Not guaranteed to be monotonic -> avoid using it.
    static_assert(std::chrono::high_resolution_clock::period::num == 1);
    static_assert(std::chrono::high_resolution_clock::is_steady);

    // The UTC clock represents UTC.
    // It measures time since 00:00:00 UTC, Thursday, 1 January 1970, including leap seconds.
    // std::chrono::utc_clock

    // The TAI (International Atomic Time) clock.
    // std::chrono::tai_clock

    // The GPS clock (Global Positioning Time).
    // std::chrono::gps_clock

    // The file clock is alias for std::filesystem::file_time_type.
    static_assert(std::chrono::file_clock::period::num == 1);
    static_assert(not std::chrono::file_clock::is_steady);

    // A duration consists of a span of time, defined as some number of ticks of some time unit.
    // For example, "42 seconds" could be represented by a duration consisting of 42 ticks of a 1-second time unit.
    {
        constexpr std::chrono::duration<int, std::ratio<60>> duration_3_minutes = std::chrono::minutes(3);
        static_assert(duration_3_minutes.count() == 3);
        static_assert(std::chrono::seconds(10).count() == 10);
        constexpr std::chrono::duration<int, std::ratio<1>> duration_seconds = std::chrono::duration_cast<std::chrono::seconds>(duration_3_minutes);
        static_assert(duration_seconds.count() == 180);
    }

    // A time point is a duration of time that has passed since the epoch of a specific clock.
    std::chrono::time_point<std::chrono::system_clock> tp_now = std::chrono::system_clock::now();

    std::chrono::time_point<std::chrono::system_clock,std::chrono::minutes> tp_now_minutes;;
    tp_now_minutes = std::chrono::time_point_cast<std::chrono::minutes,std::chrono::system_clock>(tp_now);

    std::chrono::duration<int,std::ratio<1>> d_seconds = tp_now_minutes.time_since_epoch();
    assert(d_seconds.count() > 1784187900);

    auto next_second = d_seconds + std::chrono::seconds(1);
    assert(next_second.count() == d_seconds.count() + 1);

    {
        // Parses a timestamp that approximately matches the RFC 3339 standard.
        // Example of a timestamp in Zulu time:    2023-11-19T14:59:37.420Z
        // Example of a timestamp with a timezone: 2023-11-19T15:59:37.420+01:00

        const std::string timestring{"2023-11-19T15:59:37.420+01:00"};
        std::istringstream iss(timestring);

        // Parsing format for a date/time stamp with specified time zone.
        // The time zone may also be indicated with a "Z" at the end: Zulu time = UTC.
        std::string format{"%Y-%m-%dT%H:%M:%S"};
        format += timestring.ends_with('Z') ? "%Z" : "%z";
        std::chrono::time_point<std::chrono::system_clock> tp;
        // iss >> std::chrono::parse(format, tp);
    }

    {
        constexpr auto year = 2026;
        constexpr auto month = 3;
        constexpr auto day = 23;
        constexpr std::chrono::year_month_day ymd {std::chrono::year(year)/month/day};
        static_assert(ymd.year() == std::chrono::year(year));
        static_assert(ymd.month() == std::chrono::month(month));
        static_assert(ymd.day() == std::chrono::day(day));
    }

    {
        const auto time_point_utc = std::chrono::system_clock::now();
        // std::cout << "UTC time: " << time_point_utc << std::endl;
        const std::time_t time = std::chrono::system_clock::to_time_t(time_point_utc);
        std::string time_str = std::ctime(&time);
        time_str.pop_back(); // Remove the \n.
        //std::cout << "Local time: " << time_str << std::endl;
        // std::cout << std::chrono::current_zone()->to_local(time_point_utc) << std::endl;
    }

    {
        const std::time_t t = std::time(nullptr);
        const std::tm tm = *std::localtime(&t);
        std::ostringstream oss;
        // The locale determines the language etc.
        //oss.imbue(std::locale("us_EN.utf8"));
        // E.g.: Tue Mar 17 20:18:21 2026 CET
        oss << std::put_time(&tm, "%c %Z") << std::endl;
    }

}
}
