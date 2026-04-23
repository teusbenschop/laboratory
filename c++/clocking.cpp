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


#include <thread>
#include "clocking.h"

#include <cassert>

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
    // A clock consists of a starting point (epoch) and a tick rate.
    // E.g. a clock may have an epoch of 1st January 1970 UTC and tick every second.
    // The system_clock is the real-time wall clock.
    // The steady_clock is monotonic and never gets adjusted.
    // The high_resolution_clock is a clock with the shortest possible tick period.

    // A time point is a duration of time that has passed since the epoch of a specific clock.

    // A duration consists of a span of time, defined as some number of ticks of some time unit.
    // For example, "42 seconds" could be represented by a duration consisting of 42 ticks of a 1-second time unit.

    {
        const auto now = std::chrono::system_clock::now();
        const auto time_point = std::chrono::time_point_cast<std::chrono::minutes, std::chrono::system_clock>(now);
        const auto duration_minutes = time_point.time_since_epoch();
        const long minutes = duration_minutes.count();
        assert(minutes >= 29559654);
        const auto next_minute = time_point + std::chrono::minutes{1};
        assert(next_minute.time_since_epoch().count() == minutes + 1);

        const auto duration_hours = std::chrono::duration_cast<std::chrono::hours>(duration_minutes);
        const auto hours = duration_hours.count();
        assert(hours == minutes/60);
    }

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

        // std::chrono::time_point<std::chrono::system_clock> tp;
        // iss >> std::chrono::parse(format, tp);
    }

    {
        constexpr auto year_month_day {std::chrono::year(2026)/3/23};
        static_assert(year_month_day.year() == std::chrono::year(2026));
        static_assert(year_month_day.month() == std::chrono::month(3));
        static_assert(year_month_day.day() == std::chrono::day(23));
    }

    {
        const auto utc = std::chrono::system_clock::now();
        //std::cout << "UTC time: " << utc << std::endl;
        const std::time_t time = std::chrono::system_clock::to_time_t(utc);
        std::string time_str = std::ctime(&time);
        time_str.pop_back(); // Remove the \n.
        //std::cout << "Local time: " << time_str << std::endl;
        //std::cout << std::chrono::current_zone()->to_local(utc) << std::endl;
    }
}
}
