#include "installable.h"
#include "utilities.h"
#include <filesystem>
#include <iostream>

namespace installable {

std::map<std::string,int> get_extensions(const std::string& directory)
{
  std::map<std::string,int> extensions;
  {
    std::vector<std::string> paths;
    utilities::recursive_scandir(directory, paths);
    for (const auto& path : paths) {
      extensions[(std::filesystem::path(path).extension())]++;
    }
  }
  return extensions;
}

}
