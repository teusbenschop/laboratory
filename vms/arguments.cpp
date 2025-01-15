
#include "arguments.h"
#include "utilities.h"
#include <sstream>
#include <filesystem>
#include <getopt.h>
#include <stdlib.h>

namespace arguments {


Arguments::Arguments()
{
  std::filesystem::path build_path (utilities::home_dir());
  build_path /= "Desktop/tfs_ums3_build";
  build_directory = build_path;
  std::filesystem::path install_path (utilities::home_dir());
  install_path /= "Desktop/UMS3 proxy installation";
  installable_directory = install_path;
}


Arguments parse(const int argc, char* argv[])
{
  Arguments arguments {};
  int c {};
  // The colon below indicates that the parameter has an argument and is not a switch.
  while ((c = getopt(argc, argv, "h?b:i:")) != -1) {
    switch (c) {
      case 'b':
        arguments.build_directory = optarg;
        continue;
        
      case 'i':
        arguments.installable_directory = optarg;
        continue;
        
      default :
        std::stringstream ss {};
        ss << "Usage: " << argv[0] << " [-b <build directory>] [-i <installable directory>]\n\n";
        ss << "The following parameters will be recognized:\n";
        ss << "[-b <build directory>]\n";
        ss << " Indicates the directory where the Visual Studio build is located.\n";
        ss << "[-i <installable directory>]\n";
        ss << " Indicates the directory where the VMS installable has been unzipped.\n";
        throw std::runtime_error(std::move(ss).str());
    }
    break;
  }
  
  // Check that the IP address has been set:
  if (arguments.build_directory.empty())
    throw std::runtime_error("Error: Build directory missing.");
  if (arguments.installable_directory.empty())
    throw std::runtime_error("Error: Installable directory missing.");
  
  return arguments;
}

}
