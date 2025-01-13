
#pragma once

#include <string>

namespace file {

struct File {
  const std::string name {};
  const std::string extension {};
  const std::string parent {};
  const int size {};
  const std::string path {};
};

}
