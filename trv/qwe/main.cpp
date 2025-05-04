#include "main.h"
#include <cstdlib>
#include <string>
#include <array>
#include <fstream>
#include <sstream>

int main()
{
  //constexpr const std::array<int,2> tdis_ids {456, 654};
  constexpr const int tdis_id {456};
  constexpr const int start_hour {0};
  constexpr const int stop_hour {23};
  constexpr const int start_minute {0};
  constexpr const int stop_minute {59};
  constexpr const int lower_speed {5};
  constexpr const int upper_speed {50};
  int speed {lower_speed};
  int delta {1};

  // Example speed CSV line:
  //   1;2345;2018-10-05T00:00:01.785;26.4;0
  std::stringstream speed_ss;
  for (int hour {start_hour}; hour <= stop_hour; hour++) {
    for (int minute {start_minute}; minute <= stop_minute; minute++) {
      for (int second {0}; second <= 59; second++) {
        speed_ss << "1;" << tdis_id << ";2025-02-05";
        speed_ss << "T" << std::setw(2) << std::setfill('0') << hour;
        speed_ss << ":" << std::setw(2) << std::setfill('0') << minute;
        speed_ss << ":" << std::setw(2) << std::setfill('0') << second;
        speed_ss << ".000;";
        // Next speed.
        if (speed >= upper_speed)
          delta = -1;
        if (speed <= lower_speed)
          delta = +1;
        speed += delta;
        const float meters_per_second = static_cast<float>(speed) /3.6f;
        speed_ss << std::fixed << std::setprecision(1) << meters_per_second << ";0" << std::endl;
      }
    }
  }

  // Write it to file.
  {
    constexpr const std::string_view speed_csv {"/tmp/speed.csv"};
    std::ofstream file;
    file.open(speed_csv, std::ios::binary | std::ios::trunc);
    file << speed_ss.str();
    file.close ();
  }
  
  // Example status CSV line:
  //   2345;2018-04-27T08:56:00.000000Z;0;0
  std::stringstream status_ss;
  for (int hour {start_hour}; hour <= stop_hour; hour++) {
    for (int minute {start_minute}; minute <= stop_minute; minute++) {
      status_ss << tdis_id << ";2025-02-05";
      status_ss << "T" << std::setw(2) << std::setfill('0') << hour;
      status_ss << ":" << std::setw(2) << std::setfill('0') << minute;
      status_ss << ":00.000;0;0" << std::endl;
    }
  }

  // Write it to file.
  {
    constexpr const std::string_view status_csv {"/tmp/status.csv"};
    std::ofstream file;
    file.open(status_csv, std::ios::binary | std::ios::trunc);
    file << status_ss.str();
    file.close ();
  }

  return EXIT_SUCCESS;
}
