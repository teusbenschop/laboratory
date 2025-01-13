
#pragma once

#include <string>

namespace arguments {

// The arguments passed to the tool.
class Arguments {
public:
  Arguments();
  std::string build_directory {};
  std::string installable_directory {};
private:
  std::string home();
};

// Parses the arguments passed to the tool via the commandline.
Arguments parse(const int argc, char* argv[]);

}
