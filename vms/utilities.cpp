#include "utilities.h"
#include "file.h"
#include <filesystem>
#include <iostream>
#include <dirent.h>

namespace utilities {

std::vector <std::string> scandir (const std::string& folder)
{
  std::vector <std::string> files;
  // This older way of reading directories is used.
  // because the std::filesystem gave segmentation faults.
  DIR * dir = opendir (folder.c_str());
  if (dir) {
    dirent * direntry;
    while ((direntry = readdir (dir)) != nullptr) {
      const std::string name = direntry->d_name;
      // Exclude undesirable names.
      if (name == ".") continue;
      if (name == "..") continue;
      if (name == ".DS_Store") continue;
      // Store the name.
      files.push_back (name);
    }
    closedir (dir);
  }
  std::sort (files.begin(), files.end());
  return files;
}


void recursive_scandir (const std::string& folder, std::vector <std::string>& paths)
{
  std::vector <std::string> files = scandir (folder);
  for (const auto& file : files) {
    const std::filesystem::path path = std::filesystem::path(folder).append(file);
    if (std::filesystem::is_directory(path)) {
      recursive_scandir (path, paths);
    } else {
      paths.push_back (path);
    }
  }
}


std::vector<file::File> paths_to_files(const std::vector<std::string>& paths) // Todo
{
  std::vector<file::File> files{};
  for (const auto& p : paths) {
    const std::filesystem::path path (p);
    files.emplace_back (path.filename(), path.extension(), path.parent_path(),
                        static_cast<int>(std::filesystem::file_size(path)));
  }
  return files;
}


}
