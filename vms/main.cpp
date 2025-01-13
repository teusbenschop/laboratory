#include <exception>
#include <iostream>
//#include <stdexcept>
#include <string>
#include <vector>
#include "arguments.h"
#include "installable.h"


int main (int argc, char *argv[])
{
  std::cout << "VMS checker" << std::endl;
  try {
    const arguments::Arguments arguments {arguments::parse(argc, argv)};
    std::cout << "Build directory: " << arguments.build_directory << std::endl;
    std::cout << "Installable directory: " << arguments.installable_directory << std::endl;

    std::cout << std::endl;
    const auto extensions = installable::get_extensions(arguments.installable_directory);
    std::cout << "Extensions in the installable:" << std::endl;
    for (const auto& extension : extensions)
      std::cout << extension.second << " x " << extension.first << std::endl;

    return EXIT_SUCCESS;
  }
  catch (const std::exception& exception) {
    std::cerr << exception.what() << std::endl;
  }
  return EXIT_FAILURE;
}
