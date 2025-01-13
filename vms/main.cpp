#include <exception>
#include <iostream>
//#include <stdexcept>
#include <string>
#include <vector>
#include "arguments.h"
#include "installable.h"
#include "utilities.h"
#include "file.h"


// Whether to print the extensions of the intallable files.
constexpr const bool list_installable_extensions {false};

// Whether to print whether every installable is obtainable from the built source.
constexpr const bool list_installable_from_source {true};


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

    // Print all extensions in the installable, for info.
    if (list_installable_extensions) {
      std::cout << std::endl;
      std::cout << "Extensions in the installable:" << std::endl;
      for (const auto& extension : get_extensions(installable_files))
        std::cout << extension.second << " x " << extension.first << std::endl;
    }

    // Function to convert a File object to a clear human-readable text.
    const auto print_file = [](const file::File& file) {
      std::string path = file.parent;
      if (const size_t pos = path.find(utilities::home_dir()); pos == 0) {
        path.erase (0, utilities::home_dir().size() + 1);
      }
      path.append ("/");
      path.append (file.name);
      return path;
    };
    
    // Check that all files in the installable can be found in the built source code.
    if (list_installable_from_source) {
      std::cout << std::endl;
      for (const auto& installable_file : installable_files) {
        std::cout << std::endl;
        std::cout << "Checking installable: " << print_file(installable_file) << std::endl;
        int matches {0};
        for (const auto& build_file : build_files) {
          if (installable_file.name != build_file.name)
            continue;
          std::cout << "Source candidate: " << print_file(build_file) << std::endl;
          if (installable_file.size != build_file.size) {
            std::cout << "- different size: installable = " << installable_file.size << " candidate = " << build_file.size << std::endl;
            continue;
          }
          const std::string installable_contents {utilities::get_contents(installable_file.path)};
          const std::string build_contents {utilities::get_contents(build_file.path)};
          if (installable_contents == build_contents) {
            std::cout << "- exact match" << std::endl;
            matches++;
          } else {
            size_t i {0};
            for (; i < installable_contents.size(); i++)
              if (installable_contents.at(i) != build_contents.at(i))
                break;
            std::cout << "- different content starting at byte " << i << " out of " << installable_contents.size() << std::endl;
          }
        }
        if (!matches)
          std::cout << "A matching file in the built source was not found" << std::endl;
      }
    }
    
    // Ready.
    return EXIT_SUCCESS;
  }
  catch (const std::exception& exception) {
    std::cerr << exception.what() << std::endl;
  }
  return EXIT_FAILURE;
}
