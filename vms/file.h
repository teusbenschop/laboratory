
#pragma once

#include <string>

namespace file {

struct File {
  const std::string name {};
  const std::string extension {};
  const std::string path {};
  const int size {};
};

}
