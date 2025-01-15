
#pragma once

#include <string>
#include <vector>
namespace file { struct File; };

namespace utilities {

std::vector <std::string> scandir (const std::string& folder);
void recursive_scandir (const std::string& folder, std::vector <std::string> & paths);
std::vector<file::File> paths_to_files(const std::vector<std::string>& paths);
std::string get_contents(const std::string& path);
std::string home_dir();
std::string path_to_ildasm (const file::File& file);

}
