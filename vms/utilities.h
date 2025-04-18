
#pragma once

#include <string>
#include <vector>
namespace file { struct File; };

namespace utilities {

std::vector <std::string> scandir (const std::string& folder);
void recursive_scandir (const std::string& folder, std::vector<std::string>& paths, const bool include_dirs);
std::vector<file::File> paths_to_files(const std::vector<std::string>& paths);
std::string get_contents(const std::string& path);
std::string home_dir();
std::string path_to_ildasm (const file::File& file);
std::string normalize (const std::string input);
int word_similarity (std::string oldstring, std::string newstring);
std::string replace (const std::string& search, const std::string& replace, std::string subject);
std::vector <std::string> explode (const std::string& value, char delimiter);

}
