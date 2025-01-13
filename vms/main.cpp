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

    // Function to get distinct extensions from a container of file::File objects.
    const auto get_extensions = [](const auto& files) {
      std::map<std::string,int> extensions;
      for (const auto& file : files) {
        extensions[file.extension]++;
      }
      return extensions;
    };

    std::cout << std::endl;

    // Print all extensions in the installable, for info.
    std::cout << "Extensions in the installable:" << std::endl;
    for (const auto& extension : get_extensions(installable_files))
      std::cout << extension.second << " x " << extension.first << std::endl;

    std::cout << std::endl;

    // Function to convert a File object to a clear human-readable text.
    const auto print_file = [&arguments](const file::File& file) {
      std::string path = file.path;
      if (const size_t pos = path.find(arguments.build_directory); pos == 0) {
        path.erase (0, arguments.build_directory.size() + 1);
      }
      if (const size_t pos = path.find(arguments.installable_directory); pos == 0) {
        path.erase (0, arguments.installable_directory.size() + 1);
      }
      path.append ("/");
      path.append (file.name);
      return path;
    };
    
    // Check that all files in the installable can be found in the built source code.
    for (const auto& installable_file : installable_files) {
      std::cout << "Checking installable: " << print_file(installable_file) << std::endl;
      int matches = 0;
      for (const auto& build_file : build_files) {

        const auto match = [&installable_file, &build_file](const bool size) {
          if (installable_file.name != build_file.name)
            return false;
          if (size)
            if (installable_file.size != build_file.size)
              return false;
          return true;
        };
        
        if (match(true)) {
          std::cout << "Original found: " << print_file(build_file) << std::endl;
          matches++;
          continue;
        }

        if (match(false)) {
          std::cout << "File with different size: " << print_file(build_file) << std::endl;
          matches++;
          continue;
        }
       
      }
      if (!matches)
        std::cout << "A matching file in the built source was not found" << std::endl;
    }
    
    // Ready.
    return EXIT_SUCCESS;
  }
  catch (const std::exception& exception) {
    std::cerr << exception.what() << std::endl;
  }
  return EXIT_FAILURE;
}
