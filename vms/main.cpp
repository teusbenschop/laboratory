#include <exception>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "arguments.h"
#include "installable.h"
#include "utilities.h"
#include "file.h"


// Whether to print the extensions of the intallable files.
constexpr const bool list_installable_extensions {false};

// Which file extensions to check below, skipping other extensions.
const std::set<std::string> installable_extensions {
  // ".InstallLog",
  // ".InstallState",
  // ".bat",
  // ".config",
  // ".csv",
  ".dll",
  // ".docx",
  ".exe",
  // ".ini",
  // ".pdb", Program database with debug information.
  // ".sdf", Visual Studio code browser database.
  // ".txt",
  // ".xlsx",
};

// Print whether all distinct installables are the same.
// Example: The file foo.dll occurs multiple times in the installable.
//          Prints whether they are all the same.
constexpr const bool list_installable_consistency {false};

// Whether to print whether every installable is obtainable from the built source.
constexpr const bool list_installable_from_source_detailed {false};
constexpr const bool list_installable_from_source_summary {false};

// Whether to print a script for disassemble dll and exe files on Windows.
constexpr const bool list_disassembler_commands {true};

// Normalize non-ASCII characters in path names.
constexpr const bool normalize_non_ascii_paths {false};

int main (int argc, char *argv[])
{
  std::cout << "VMS Proxy source and installable checker" << std::endl;
  try {
    // Parse arguments or take defaults.
    const arguments::Arguments arguments {arguments::parse(argc, argv)};
    std::cout << "Build directory: " << arguments.build_directory << std::endl;
    std::cout << "Installable directory: " << arguments.installable_directory << std::endl;

    // Produce the File containers for the build and for the installable.
    const auto get_paths = [](const std::string& directory) {
      std::vector <std::string> paths;
      utilities::recursive_scandir(directory, paths, false);
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
    
    
    // Check the internal consistency of the distinct installable files.
    // Limit check to the given file extensions.
    if (list_installable_consistency) {
      std::cout << std::endl;
      // Track distinct filenames already checked.
      std::set<std::string> checked_filenames{};
      // Per distinct filename, track the exact matches plus the associated info to output.
      std::map<std::string, std::map<int, std::string>> filename_matches_info {};
      // Iterate over the installables.
      for (const auto& installable_file : installable_files) {
        // Skip unlisted extension.
        if (!installable_extensions.contains(installable_file.extension))
          continue;
        // Skip filename already checked with 100% match.
        if (checked_filenames.contains(installable_file.name))
          continue;
        // Container for the info to generate.
        std::stringstream ss{};
        ss << std::endl;
        ss << "Checking distinct installable: " << installable_file.name << std::endl;
        // Iterate over the installables and check for exact or partial match in size and contents.
        int matches {0};
        int diff_sizes {0};
        int diff_contents {0};
        for (const auto& check_file : installable_files) {
          if (installable_file.name != check_file.name)
            continue;
          if (installable_file.size != check_file.size) {
            ss << "Candidate " << print_file(check_file) << " has different size: installable = " << installable_file.size << " candidate = " << check_file.size << std::endl;
            diff_sizes++;
            continue;
          }
          const std::string installable_contents {utilities::get_contents(installable_file.path)};
          const std::string check_contents {utilities::get_contents(check_file.path)};
          if (installable_contents == check_contents) {
            matches++;
          } else {
            size_t i {0};
            for (; i < installable_contents.size(); i++)
              if (installable_contents.at(i) != check_contents.at(i))
                break;
            ss << "Candidate " << print_file(check_file) << " has different content starting at byte " << i << " out of " << installable_contents.size() << std::endl;
            diff_contents++;
          }
        }
        if (matches)
          ss << "The installable contains " << matches << " exact identical files" << std::endl;
        if (diff_sizes)
          ss << "The installable contains " << diff_sizes << " files with the same name but a different size" << std::endl;
        if (diff_contents)
          ss << "The installable contains " << diff_contents << " files with the same name and size but a different content" << std::endl;
        // Store the info per filename, per match count, for later reference.
        filename_matches_info[installable_file.name][matches] = std::move(ss).str();
        // In case of 100% match, store the filename as been done.
        if ((diff_sizes == 0) && (diff_contents == 0))
          checked_filenames.insert(installable_file.name);
      }
      // Print the output.
      // If there's multiple comparisons, print the best comparison, that is with most matches.
      for (const auto& element1 : filename_matches_info) {
        const std::map<int, std::string>& matches_info = element1.second;
        const std::string& info = std::prev(matches_info.end())->second;
        std::cout << info << std::endl;
      }
    }
    
    
    // Elaborate check whether the files in the installable can be found in the built source code.
    // Limit checks to the given file extensions.
    if (list_installable_from_source_detailed) {
      std::cout << std::endl;
      for (const auto& installable_file : installable_files) {
        if (!installable_extensions.contains(installable_file.extension))
          continue;
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

    
    // Summarizing check whether the files in the installable
    // can be found in the built source code.
    // Limit checks to the given file extensions.
    if (list_installable_from_source_summary) {
      std::map<std::string,int> filenames{};
      std::map<std::string,int> name_matches{};
      std::map<std::string,int> size_matches{};
      std::map<std::string,int> content_matches{};
      for (const auto& installable_file : installable_files) {
        if (!installable_extensions.contains(installable_file.extension))
          continue;
        filenames[installable_file.name]++;
        bool name_match {false};
        bool size_match {false};
        bool content_match {false};
        for (const auto& build_file : build_files) {
          if (installable_file.name != build_file.name)
            continue;
          name_match = true;
          if (installable_file.size != build_file.size) {
            continue;
          }
          size_match = true;
          const std::string installable_contents {utilities::get_contents(installable_file.path)};
          const std::string build_contents {utilities::get_contents(build_file.path)};
          if (installable_contents == build_contents) {
            content_match = true;
          }
        }
        if (name_match)
          name_matches[installable_file.name]++;
        if (size_match)
          size_matches[installable_file.name]++;
        if (content_match)
          content_matches[installable_file.name]++;
      }
      std::cout << "The installable file header indicates the filenames from the installable" << std::endl;
      std::cout << "The count header indicates how often the installable occurs in the installaton package" << std::endl;
      std::cout << "The name match header indicates how often the name was found in the built sources" << std::endl;
      std::cout << "The size match header indicates how often a file with the same name and ame size was found in the built sources" << std::endl;
      std::cout << "The content match header indicates how often the exact file was found in the built sources" << std::endl;
      std::cout << std::endl;
      constexpr const char separator {' '};
      constexpr const int name_width {60};
      constexpr const int num_width {15};
      std::cout << std::setw(name_width) << std::setfill(separator) << "Installable file" << std::setw(num_width) << std::setfill(separator) << "count" << std::setw(num_width) << std::setfill(separator) << "name match" << std::setw(num_width) << std::setfill(separator) << "size match" << std::setw(num_width) << std::setfill(separator) << "content match" << std::endl;
      std::cout << std::endl;
      for (const auto& filename : filenames) {
        std::cout << std::setw(name_width) << std::setfill(separator) << filename.first << std::setw(num_width) << std::setfill(separator) << filename.second << std::setw(num_width) << std::setfill(separator) << name_matches[filename.first] << std::setw(num_width) << std::setfill(separator) << size_matches[filename.first] << std::setw(num_width) << std::setfill(separator) << content_matches[filename.first] << std::endl;
      }
    }

    
    // Whether to print disassembler commands for object files and executables.
    if (list_disassembler_commands) {
      const auto print_command = [] (const auto& file) {
        const auto path = file.parent + "/" + file.name;
        std::cout << "ildasm /source /pubonly /noca /nobar ";
        std::cout << std::quoted(path);
        std::cout << " /out=" << std::quoted(utilities::path_to_ildasm(file));
        std::cout << std::endl;
        std::cout << "IF %ERRORLEVEL% NEQ 0 pause" << std::endl;
      };
      const std::set<std::string> extensions { ".dll", ".exe" };
      std::cout << std::endl;
      for (const auto& file : installable_files)
        if (extensions.contains(file.extension))
          print_command(file);
      for (const auto& file : build_files)
        if (extensions.contains(file.extension))
          print_command(file);
      std::cout << "echo Ready" << std::endl;

    }


    // Whether to normalize non-ASCII parts of file paths.
    if (normalize_non_ascii_paths) {
      const auto get_paths = [&arguments] () {
        std::vector <std::string> paths;
        utilities::recursive_scandir(arguments.build_directory, paths, true);
        utilities::recursive_scandir(arguments.installable_directory, paths, true);
        return utilities::paths_to_files(paths);
      };
      const std::vector<file::File> paths = get_paths();
      std::cout << "Scanning " << paths.size() << " files" << std::endl;
      for (const auto& path : paths) {
        const auto normalize = [] (const auto& file) {
          const std::string path = file.parent + "/" + file.name;
          if (std::filesystem::exists(std::filesystem::path(path))) {
            const std::string normal_path = utilities::normalize (path);
            if (normal_path != path) {
              std::cout << "From: " << path << std::endl;
              std::cout << "To  : " << normal_path << std::endl;
              std::filesystem::rename (std::filesystem::path(path), std::filesystem::path(normal_path));
            }
          }
        };
        normalize(path);
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
