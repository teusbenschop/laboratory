#include "utilities.h"
#include "file.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <unicode/normlzr.h>
#include <unicode/translit.h>


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


void recursive_scandir (const std::string& folder, std::vector<std::string>& paths, const bool include_dirs)
{
  std::vector <std::string> files = scandir (folder);
  for (const auto& file : files) {
    const std::filesystem::path path = std::filesystem::path(folder).append(file);
    if (std::filesystem::is_directory(path)) {
      if (include_dirs)
        paths.push_back (path);
      recursive_scandir (path, paths, include_dirs);
    } else {
      paths.push_back (path);
    }
  }
}


std::vector<file::File> paths_to_files(const std::vector<std::string>& paths)
{
  std::vector<file::File> files{};
  for (const auto& p : paths) {
    const std::filesystem::path path (p);
    files.emplace_back (path.filename(),
                        path.extension(),
                        path.parent_path(),
                        std::filesystem::is_directory(path) ? 0 : static_cast<int>(std::filesystem::file_size(path)),
                        p);
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


// Function to convert a file object to a path to the IL deassembler file.
std::string path_to_ildasm (const file::File& file) {
  const auto path = file.parent + "/" + file.name;
  return path.substr(0, path.size() - file.extension.size()) + ".ildasm.txt";
};


std::string normalize (const std::string input)
{
  // UTF-8 std::string -> UTF-16 UnicodeString
  icu::UnicodeString source = icu::UnicodeString::fromUTF8 (icu::StringPiece (input));
  
  // Transliterate UTF-16 UnicodeString following this rule:
  // decompose, remove diacritics, recompose
  UErrorCode status = U_ZERO_ERROR;
  icu::Transliterator *accents_converter = icu::Transliterator::createInstance("NFD; [:M:] Remove; NFC", UTRANS_FORWARD, status);
  accents_converter->transliterate(source);
  
  // Case folding.
  //source.foldCase ();
  
  // UTF-16 UnicodeString -> UTF-8 std::string
  std::string result {};
  source.toUTF8String (result);
  
  // Ready.
  return result;
}


}
