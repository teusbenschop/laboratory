
#pragma once

#include <string>
#include <vector>

namespace utilities {

std::vector <std::string> scandir (const std::string& folder);
void recursive_scandir (const std::string& folder, std::vector <std::string> & paths);

}
