
#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

namespace installable {

std::map<std::string,int> get_extensions(const std::string& directory);

}
