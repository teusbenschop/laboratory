#include "utilities.h"
#include "file.h"
#include <filesystem>
#include <iostream>
#include <fstream>
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
                        static_cast<int>(std::filesystem::file_size(path)), p);
  }
  return files;
}


std::string get_contents(const std::string& path)
{
  if (!std::filesystem::exists (path))
    return std::string();
  try {
    std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    const std::streamoff filesize = ifs.tellg();
    if (filesize == 0)
      return std::string();
    ifs.seekg(0, std::ios::beg);
    std::vector <char> bytes(static_cast<size_t> (filesize));
    ifs.read(&bytes[0], static_cast<int> (filesize));
    return std::string(&bytes[0], static_cast<size_t> (filesize));
  }
  catch (...) { }
  return std::string();
}


std::string home_dir()
{
  static std::string cache {};
  if (!cache.empty())
    return cache;
  if (const char* home = getenv ("HOME"); home) {
    cache = home;
    return cache;
  }
  throw std::runtime_error("Error: Cannot find the home directory");
  

  
  
}


}
