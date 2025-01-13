#include <exception>
#include <iostream>
//#include <stdexcept>
#include <string>
#include <vector>
#include "arguments.h"
#include "installable.h"
#include "utilities.h"
#include "file.h"


int main (int argc, char *argv[])
{
  std::cout << "VMS checker" << std::endl;
  try {
    // Parse arguments or take defaults.
    const arguments::Arguments arguments {arguments::parse(argc, argv)};
    std::cout << "Build directory: " << arguments.build_directory << std::endl;
    std::cout << "Installable directory: " << arguments.installable_directory << std::endl;

    // Produce the File containers for the build and for the installable.
    const auto get_paths = [](const std::string& directory) {
      std::vector <std::string> paths;
      utilities::recursive_scandir(directory, paths);
      return utilities::paths_to_files(paths);
    };
    const std::vector<file::File> build_files = get_paths(arguments.build_directory);
    const std::vector<file::File> installable_files = get_paths(arguments.installable_directory);

    // Print all extensions in the installable, for info.
    const auto get_extensions = [](const auto& files) {
      std::map<std::string,int> extensions;
      for (const auto& file : files) {
        extensions[file.extension]++;
      }
      return extensions;
    };
    std::cout << std::endl;
    std::cout << "Extensions in the installable:" << std::endl;
    for (const auto& extension : get_extensions(installable_files))
      std::cout << extension.second << " x " << extension.first << std::endl;

    // Ready.
    return EXIT_SUCCESS;
  }
  catch (const std::exception& exception) {
    std::cerr << exception.what() << std::endl;
  }
  return EXIT_FAILURE;
}
